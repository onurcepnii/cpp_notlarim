
İlginç ama küçük bir sentaks özelliği;

	template <typename T>
	class Myclass {

	};

	template <typename T>
	class Nec {};

	int main()
	{
		Myclass<Nec<int>> mx;	// Modern Cpp'den önce böyle bir bildirim maximal munch'a takılıyordu. Hatta bazı eski kaynaklarda maximal munch'tan kaçınmak için >> arasına bir boşluk koymak gerektiğinden bahsedilir. Fakat o eskide kaldı.

		// Derleyici artık tokenizingi ona göre yapıyor, boşluk karakteri vermek zorunda değiliz.
	}


-------------------------------------

Soru: Aşağıdaki Myint bir sınıf mı?
- Hayır, Myint bir sınıf şablonu. Derleyicinin sınıf kodu yazmasını sağlayacak bir kod.

	template <typename T>
	class Myint {
	public:
		Myint(T x);
		Myint& set(T x);
		T get()const;
	private:
		T mval;
	};

	template <typename T>
	T Myint<T>::get()const	// #
	{
		return mval;
	}

# işaretli yerde T yerine U kullansaydık aynı anlama mı gelirdi yoksa farklı mı olurdu?
-Aynı anlama gelirdi. Bir problem olmazdı. Zaten yukarıda olan template'de ki ismin scope'u template kodun tamamlandığı yere kadar. Yani fonksiyonun tanımını yaparken aynı template parametresine verdiğimiz isimle aynı ismi kullanmak zorunda değiliz.



*** ÖNEMLİ


	template <typename T>
	class Myint {
	public:
		Myint(T x);
		Myint& set(T x)
		{
			Myint<T> y; ile yazmakla Myint y; yazmak aynı anlamda.
		}
		T get()const;
	private:
		T mval;
	};

	template <typename T>
	Myint<T> &Myint<T>::set(T val)	// ##
	{
		Myint veya Myint<T> aynı
	}

Üye fonksiyonun içinde sınıf şablonunun ismini yalın olarak kullandığımız zaman aslında derleyici bunu hangi specialization için instantiation yapacaksa o tür olarak görüyor.

Benzer şekilde fonksiyonun tanımını class dışında yaptığımız zaman

## : scope resolution operatörden sonra artık doğrudan class scope'dayız. Yani gerek parametreyi yazdığımız yer gerek fonksiyonun ana bloğu içinde Myint<T> demek ile Myint yazmak arasında bir fark yok. Ama fonksiyonun geri dönüş değerini yazdığımız yerde burası doğrudan class scope'da olmadığı için sınıfın kendisinden bahsederken Myint<T> yazmak zorundayız.


Burada belki şöyle bir istisna olabilir; Constructor

	template <typename T>
	class Myint {
	public:
		Myint(int);
	private:
		T mval;
	};

	template <typename T>
	Myint<T>::Myint(int)	// #
	{

	}


# : normalde burada class scope içinde fakat constructor söz konusu olduğu için burada <T> kullanmamak zorundayız. Yani Myint<T>::Myint<T>(int) yazamayız!
	


----------------------------------

Sınıf şablonunun her bir specializationu için yapılan instntiation ayrı bir sınıf oluşturuyor ve bu sınıflar birbirinden tamamen bağımsız.

Yani aynı template'den oluşturulan farklı specializationlara ilişkin instantiaton'lar farklı türler oluşturuyor.

Bu durum sınıfın statik veri elemanları için de öyle.


	template <typename T>
	class Nec {
	public:
		static int ms;
	};

	template <typename T>
	int Nec<T>::ms = { 0 };

	int main()
	{
		// Nec<int>::ms 		Bu ikisi farklı. Aynı değil.
		// Nec<double>::ms
	}


-------------------------------

Bir sınıf şablonu oluştururken onu şablon olmayan bir taban sınıftan inherit ederek elde edebiliriz.

	class Base {};

	template <typename T>
	class Der : public Base {

	};


Aşağıda da herhangi bir problem yok, legal.

	template <typename T>
	class Base {};

	class Der : public Base<int> {

	};


//////////////////////////////

Bu farklı bir durum. Der<int> açılımını kullansak bunun taban sınıfı hangi sınıf olacak? Base<int> açılımı olacak.

Der<string> olsa onun da taban sınıfı Base<string> olacak.


	template <typename T>
	class Base {};

	template <typename T>
	class Der : public Base<T> {

	};

	int main()
	{
		Der<int> 
	}


Bir önemli yer daha;
	

	template <typename T>
	class Base {
	public:
		void foo();
	};


	class Der : public Base<int> {
		void bar()
		{
			foo();	// herhangi bir sorun yok.
		}
	};


Fakat eğer şöyle yaparsak;
	

	template <typename T>
	class Base {
	public:
		void foo();
	};

	template <typename T>
	class Der : public Base<T> {
		void bar()
		{
			foo();	// foo identifier not found.
		}
	};

Böyle bir durumda foo ismini niteleyerek kullanmak zorundayız. Nitelemeden kullandığımızda derleyici bu foo'yu taban sınıfında aramayacak. Örneğin this->foo() veya Base::foo() yazsaydık sentaks hatası olmazdı.



//////////////////////////////


	template <typename T>
	class Nec {};

	int main()
	{
		Nec<int> x;
		Nec<int> y;		// x ve y aynı sınıf türünden.
	}


//////////////////////////////

Sınıf şablonları da nested type'a sahip olabilir.


	template <typename T>
	class Myclass {
	public:
		class Nested {

		};
	};

Myclass bir sınıf şablonu olduğu için her farklı specialization'a karşılık gelecek instantiation için ayrı bir Nested class var.

Örneğin;
	
	int main()
	{
		Myclass<int>::Nested ile Myclass<double>::Nested ayrı type'lar.
	}


Böyle durumlarda Nested class'ı sınıf içinde bildirip tanımını sınıf dışında da yapabiliriz.


	template <typename T>
	class Myclass {
	public:
		class Nested;
	};

	template <typename T>
	class Myclass<T>::Nested {

	};


------------------------------------------------

Bu doğrudan sentaks hatası;

	template <int N>
	class Myclass {

	};

	template <long N>
	class Myclass {

	};

Fakat sonradan Modern Cpp ile dile bir özellik eklediler;s

	template <auto N>	// #
	class Myclass {};

	int main()
	{
		Myclass<10> 	m1;	// legal int türden
		Myclass<5u> 	m2;	// legal unsigned int türden
		Myclass<3.4>	m3;	// legal double türden (double cpp20 ile legal hale geldi)
	}

# : bu hala non-type parameter. Fakat özelliği, deduction olacak template argümanı olarak kullanılan ifadenin türü neyse biz aslında o türden bir değer kullanmış olacağız.


///////////////////////////////

	template <typename T, T X>	// #
	class Myclass {

	};

	int main()
	{
		Myclass<int, 5>
		Myclass<char, 'A'>
		Myclass<double, 4.5>	// eğer c++ 20 yi destekliyorsa
	}

# : T yerine int kullansaydık nasıl non-type parametre olacaktı ise şimdi de non-type parametre.


-------------------------------------------



Non-Type Parametre Türleri
--------------------------

Non-Type Parametrenin Türü konusunda C++20 ile çok ciddi gelişmeler yaşandı.

Yani eskiden non-type parametre olarak kullanılamayacak türler şimdi non-type parametre olarak kullanılabiliyor.

Fakat şuan C++20 öncesinden bahsedeceğiz.



Şunların hepsi non-type parametre olabilir; (buna enum türleri de dahil)
	
	enum Color {blue, black, white};
	
*	template <Color N>
	class Nec {};

*	template <int N>
	class Nec{};

*	template <int* p>
	class Myclass{};

template argümanı olarak bizim bir int nesne adresi vermemiz lazım. Ama özellikle pointer olması durumunda o pointera karşılık gelecek template argümanı olan adresin statik ömürlü bir nesne adresi olması gerekiyor.

Örneğin;
	
	template <int* p>
	class Myclass {

	};

	int main()
	{
		int x = 5;

		Myclass<&x> m1;	// hata. pointer to int olmadığı için değil statik ömürlü bir pointer to int olmadığı için.

		//fakat static int x = 5; olsaydı veya global namespace de tanımlansa(ikisi de aynı yere çıkıyor) bir sentaks hatası olmayacak.
	}

*	template <int &r>
	class Myclass {};

	int g{};
	
	int main()
	{
		Myclass<g> m1;
	}


* 	template <int (*pf)(int)> 	//function pointer türü
	class Myclass {
		
		void foo()
		{
			int x = pf(10);
		}
	};

	int f1(int);
	int f2(int);

	int main()
	{
		Myclass<f1> m1;

		m1.foo();
	}




-------------------------------------
İlerde ayrıntılı göreceğiz;

Sınıfların statik üye fonksiyonlarının this pointeri yok. O yüzden sınıfların statik üye fonksiyonlarını gösterecek function pointer türleri bildiğimiz C'den gelen function pointer türleri.

	class Myclass {
	public:
		static int foo(int);
	};

	int main()
	{
		int (*fptr)(int) = &Myclass::foo;	// #

		int x = fptr(20);
	}

Fakat eğer bu fonksiyon (#) statik üye fonksiyon değilse artık böyle bir fonksiyonun adresini tutacak pointer böyle bir pointer değil. Bu iki tür arasında uyumsuzluk var.

İşte C'de olmayan C++'da olan "Member Function Pointer" dediğimiz ayrı bir tür var.

	class Myclass {
	public:
		int foo(int);
	};

	int main()
	{
		auto fp = &Myclass::foo;	// aslında auto type deduction kullanmasak fonksiyon türünü aşağı satırdaki gibi yazmamız gerekiyordu

		int (Myclass:: * fp)(int) = &Myclass::foo;
	}


Hatta hatta şimdi fp'nin gösterdiği fonksiyonu çağırabilmemiz için(non-static member function olduğu için) bir nesneye ihtiyacımız var.

Örneğin : 	Myclass m;

Öyle bir sentaksa ihtiyaç duyuluyor ki m nesnesini *this olarak kullanacak ve fp'nin gösterdiği fonksiyonun çağırılmasını sağlayacak.

	.* operatörü

	int (Myclass::*fp)(int) = &Myclass::foo;
	Myclass m;

	(m.*fp)(23)

MADEM BUNLARI ŞİMDİ TAM OLARAK GÖRMEYECEĞİZ NEDEN ARAYA SOKTUK?
ÇÜNKÜ;

***
	Non-type parametre member function pointer türü de olabilir.

Örneğin;
	
	struct Myclass {
		int foo(int);
	};

	template <int (Myclass::*f)(int)>	// Non-Type Parameter
	class Nec {
		// f değeri kullanıldığında Myclass sınıfının foo fonksiyonunun adresini kullanmış olacağız.
	};

	int main()
	{
		Nec<&Myclass::foo> mynec;
	}



-----------------------------------------------

01.11


Bir template'in argümanını bir yazı yapmak istiyoruz diyelim.

Öyle bir sınıf şablonu olsun ki biz sınıf şablonunda bu yazıyı non-type parametreye karşılık gelen argüman yapalım ve böylece bu sınıfın kodu içinde bizde bir veya birden fazla yerde bu yazıyı kullanabilelim.

	template <const char* p>
	class Nec {

		void foo()
		{
			std::cout << p;
		}
	};

	int main()
	{
		char s[] = "onur cepni";

		Nec<s> y;	// hata verir. statik ömürlü olmayan bir diziyi non-type argument olarak kullanamazsın hatası verir.
	}

------------------------
	
	char s[] = "onur cepni";

	int main()
	{
		Nec<s> y; // şimdi olur, statik ömürlü çünkü s.

		y.foo();

		----------------------
		// ama string literalinin kendisini template argument olarak kullanamayız. yani şöyle;

		Nec<"onur cepni"> x;	// bu şekilde yapamıyoruz..
	}



------------------------------------------

	template <int I, bool B>
	class Myclass {};

	int main()
	{
		Myclass<20, false> x; // burada bir hata yok

	-------------------------

		Myclass<sizeof(int), sizeof(int) == 4> mx; // derleyiciye göre değişiyor açılım. Yani 4, false açılımı da olabilir vs. vs.

		Myclass<sizeof(int), sizeof(int) > 2 > mx;	// sentaks hatası. aslında biz ikinci argümanda büyüktür operatörü kullandık fakat onu template in parantezi ile karıştırdı derleyici. bu durumlarda parantez içine alarak bu sorunun üzerinden gelebiliriz.

		Myclass<sizeof(int), (sizeof(int) > 2)> mx;	// legal
	}



-------------------------------------------------

***
 Container sınıfların karşılaştırma operatör fonksiyonları lexicographical compare yapıyorlar. 

 	int main()
 	{
 		std::vector<int> vx(100'000, 1);	// 100.000 tane 1 değerinde int
 		std::vector<int> vy{2};		// 1 tane 2 değerinde int

 		if(vy > vx) 
 			std::cout << "dogru";
 	}

Lexicographical compare yaptığı için doğru yazacak.


-------------------------------------------------

Mülakat Sorusu:

Öyle bir less fonksiyonu yaz ki iki tane dizi alsın ve bu dizileri lexicographical compare etsin. Dizilerin türleri aynı olsun fakat boyutları farklı olabilir.


	template <typename T, int N, int M>
	constexpr bool Less(const T(&x)[N], const T(&x)[M])
	{
		for(int i = 0; i < N && i < M; ++i) { // taşmaması iiçin
			if(x[i] < y[i])
				return true;

			if(y[i] < x[i])
				return false;
		}

		return N < M;
	}

	int main()
	{
		int a[] = { 1, 5, 8 };
		int b[] = { 1, 5, 8, 6 };

		Less(a, b);	// template argument deduction ile explicit template argümanı kullanmasak da fonksiyonu bu şekilde çağırmış olacağız: Less<int, 3, 5>(a, b);

	}




---------------------------------------------

Explicit Specialization (Full Specialization)

---------------------------------------------

Bir template'in bir specialization'u template argümanlarının yerine konulmasıyla ortaya çıkan tür.

	template <typename T>
	void Swap(T&, T&);

	int main()
	{
		Swap<int>;	// explicit specialization
	}

Normalde specialization için derleyici bizim template'imizi kullanıyor. Oradan instantiation yapıyor. Fakat şimdi biz derleyiciye diyoruz ki falanca tür için specialization oluşturduğunda benim sana verdiğim alternatif kodu kullanacaksın.

Yani primary template'den instantiation yapmak yerine instantiation zaten benim sana verdiğim kod olacak.

Burada iki noktayı iyi anlamamız gerekiyor:

-Sentaks
-Ne işe yarayacak?


Bir örnek;

	template <typename T>
	getmax(T x, T y)
	{
		return x > y ? x : y;
	}

	int main()
	{
		std::string x = "zeynep";
		std::string y = "fuat";

		std::cout << getmax(x, y) << "\n";
	}


ama şimdi kodun şöyle olduğunu düşünelim;


	int main()
	{
		const char* x = "zeynep";
		const char* y = "fuat";

		std::cout << getmax(x, y) << "\n";
	}

şimdi aynı sonucu verecek mi?
-Hayır. 

Çünkü derleyicinin oluşturduğu kodda burada fonksiyonun parametre değişkenleri pointer değişkenler olacak, biz yazıları değil pointerları karşılaştırmış olacağız.


O zaman şimdi biz şunu istiyor olalım: hey derleyici eğer burada T türü const char* ise primary template'den bir instantiation yapmak yerine benim verdiğim specialization'u kullanacaksın. işte buna explicit specialization ya da full specialization deniyor.


Sentaks şöyle:

template <typename T>
T getmax(T x, T y)
{
	return x > y ? x : y;
}

const char* türü için getmax'ın explicit specialization'u.
	template <>	// buna bazı programcılar diamond diyor.
	const char* getmax<const char*>(const char* p)	// #
	{
		return std::strcmp(p1, p2) > 0 ? p1 : p2;
	}

# : Aslında burada açısal parantez de mecburi değil. Zaten parametrelereden deduction yapılabildiği için const char* getmax(const char* p1, const char* p2) şeklinde de yazabiliyoruz.


NOT: Derleyicinin instantiation yapacağı ana template'den oluşturacağı specialization'dan bahsediyorsanız o template'e "Primary Template" deniyor yaygın olarak.


-------------------------------------------


Function Template Explicit Specialization
_________________________________________


	template <typename T>
	void foo(T)
	{
		std::cout << "primary template";
	}

	template <>
	void foo(int)
	{
		std::cout << "explicit specialization for foo<int>\n";
	}

	template <>
	void foo(double)
	{
		std::cout << "explici specialization for foo<int>\n";
	}

	int main()
	{
		foo(34L);
		foo(3.4);
		foo(99);
	}


***
Fonksiyon şablonları için bazı nedenlerden dolayı explicit specialization çok tercih edilmiyor.


***
Meta Function

Compile time'da bir tür ya da bir değer hesaplamak için kullanılan koda meta function deniyor. İsmi function ama tipik olarak sınıflar ile gerçekleştiriliyor.


	template <std::size_t N>
	struct Factorial {
		static const std::size_t value = N > 1 ? N * Factorial::<N - 1>::value: 1;
	};

	template <>
	struct Factorial <0> {
		static const std::size_t value = 1;
	}

	int main()
	{
		constexpr auto val = Factorial<5>::value;
	}



---------------------------------------------------



	template <typename T>
	struct Myclass {
		Myclass()
		{
			std::cout << "primary template \n";
		}
	};

	template <>
	struct Myclass<int> {

		Myclass()
		{
			std::cout  << "expl. spec. for int \n";
		}

		void bar() {}
		void baz() {}
	};

	int main()
	{
		Myclass<double> m1;
		m1.foo();
		Myclass<int> m2;

		m2.baz();
		m2.bar();
	}

-------------------------------------

	template <typename T, typename U, int N>
	struct Myclass {
		Myclass()
		{
			std::cout << "Primary Template\n";
		}
	};

	template <>
	struct Myclass<int, double, 20> {
		Myclass()
		{
			std::cout << "int, double, 20\n";
		}
	}

	int main()
	{
		Myclass<int, float, 20> m1;		//primary template
		Myclass<int, double, 21> m2;	//primary template
		Myclass<int, double, 20> m3;	//explicit template
	}


-------------------------------------
Compile time da ki recursivity ayrı, runtime da ki recursivity ayrı.

***
Explicit specialization'u gösteren en güzel örneklerden birisi çok meşhur bir C++ programlama sorusu.

Soru : Öyle bir kod yazın ki 0'dan 100'e kadar olan sayıları yazdırın ama döngü kullanılmasın.

Cevaplardan birini daha önce gördük. 

	template <int N>
	struct Print : Print<N - 1> {

	};

	int main()
	{
		Print<100> x;
	}

fakat böyle yazdığımızda bu 0 a gelince durmaz ki. Çünkü 0 a gelince de -1 i çağırır ve böyle gider. O halde 0 için explicit specialization yazarız ve durdururuz.



	template <int N>
	struct Print : Print<N - 1> {
		Print()
		{
			std::cout << N << ' ';
		}
	};

	template <>
	struct Print<-1> {

	};

	int main()
	{
		Print<100> x;
	}



-------------------------------------------------------

***
ÇOK ÖNEMLİ


	template <typename T>
	struct X {
		void foo(T)
		{
			std::cout << "primary template\n";
		}
	};

	template <>
	void X<int>::foo(int)
	{
		std::cout << "X<int>::foo(int)\n";
	}

	int main()
	{
		X<double> dx;
		dx.foo(23.5);
		x<int> di;
		di.foo(99);

	}


Yani X sınıfının int spec.'i söz konusu olduğunda onun foo fonksiyonu bizim yazdığımız explicit spec.'den oluşturulacak.



***
Bir önemli özellik: Gerek explicit spec. de gerek partial spec.'de primary template'in complete type olması gerekmiyor.

	template <typename>
	class Nec;

	template <>
	class Nec<int> {
	public:
		Nec()
		{
			std::cout << "int spec\n";
		}
	};

	int main()
	{
		Nec<int> x;
	}
	

Nec sınıfı in-complete type fakat int açılımı türü explicit spec. olduğu için derleyici buna izin veriyor.


---------------------------------------------

MÜLAKAT SORUSU

Fonksiyonlarda explicit spec. çok tercih edilmiyor demiştik. Hatta hatalara da sebep olabiliyorlar.


Hatta özellikle yabancı firmaların en çok sorduğu sorulardan birisi;

	template <typename T>
	void func(T)
	{
		std::cout << 1;
	}

	template <>
	void func(int*)
	{
		std::cout << 2;
	}

	template <typename T>
	void func(T*)
	{
		std::cout << 3;
	}

	int main()
	{
		int* p = nullptr;

		func(p);
	}


Cevap : 3


***
Programcıların anlaması gereken çok önemli bir nokta: Function Overload Resolution'a giren explicit specialization değil her zaman Primary Template'dir.

Bu örnekte de primary template function overload resolution'a giriyor.

Eğer function overload resolution'da primary template function seçilirse ve derleyici int* için bir spec oluştururken primary template'i değil explicit spec. i kullanarak oluşturacak.


Cevap neden 3? Function overload resolution'a 2 olan template girmiyor. 1 ve 3 arasında oluyor. Burada geçerli olan kural "Partial Ordering Rules". Hangisi daha spesifik: 3. o halde bu çağırılacak.


Sorunun ikinci şıkkı: Kodda küçük bir değişiklik yapıyoruz.

	template <typename T>
	void func(T)
	{
		std::cout << 1;
	}

	template <typename T>
	void func(T*)
	{
		std::cout << 2;
	}

	template <>
	void func(int*)
	{
		std::cout << 3;
	}

	int main()
	{
		int* p = nullptr;

		func(p);
	}

Cevap: 3

Diğer kodla bunun farkı explicit spec. in yeri az önce yukarıda iken şuan aşağıda.

Peki bir fark var mı? -VAR.

	(ÇOK ÖNEMLİ BURASI)
	Explicit spec. primary template'in altında olduğu zaman o yukarıdaki template'in(yani primary template) explicit specializationuydu.

	Ama şimdi her iki template de göründüğü için daha spesifik olan ortadaki template'in explicit specialization'u.

Bu sefer function overload resolution üstteki iki template arasında yapılacak. 2.fonksiyon daha spesifik olduğu için o kazanacak. Ama eğer burada T int ise int* olan explicit spec. kullanılacak. Yani yine 3 yazdırılacak.


Sorunun üçüncü şıkkı:

	template <typename T>
	void func(T)
	{
		std::cout << 1;
	}

	template <>
	void func(int*)
	{
		std::cout << 2;
	}

	template <typename T>
	void func(T*)
	{
		std::cout << 3;
	}

	template <>
	void func(int*)
	{
		std::cout << 4;
	}

	int main()
	{
		int* p = nullptr;
		func(p);
	}

Cevap : 4

Zaten 2 ve 4 üstündeki templatelerin (1 ve 3) explicit spec.'i. Function overload resolution'a girecek olanlar 1 ve 3. 3 kazanacak ve onun explicit spec'i kullanılacak. Yani 4.


----------------------------------------------
	
	template <typename T>
	struct Nec {
		static int ival;
	};

	template <typename T>
	int Nec<T>::ival = 9999;

	template <>
	int Nec<int>::ival = 333;

	int main()
	{
		std::cout << Nec<double>::ival << "\n";
		std::cout << Nec<int>::ival << "\n";
	}

-----------------------------------------------




----------------------

Partial Specialization

----------------------


Tüm template argümanları kümesi içinde belirli niteliklere sahip türler için verilen bir specialization.

Fonksiyon şablonlarında partial specialization yok.

Sadece sınıf ve değişken şablonlarında var.

	
	template <typename T>
	class Myclass {
	public:
		Myclass()
		{
			std::cout << "Primary Template\n";
		}

	};

	template <typename T>
	class Myclass<T*> {	// #
	public:
		Myclass()
		{
			std::cout << "Partial Spec <T *>\n";
		}
	};

	int main()
	{
		Myclass<int> m1;
		Myclass<double> m2;
		Myclass<int*> m3;
		Myclass<int**> m4;
		Myclass<int***> m5;
	}

m1 ve m2 ptr türü değil o yüzden onlar için primary template, m3 -m4 -m5 için partial spec. kullanıldı.


Sanki partial specialization'da o hangi primary template'e ilişkin ise onun template parametreleri ile aynı sayıda parametre olacakmış zannediliyor fakat böyle olabilir ya da olmayabilir. Böyle bir zorunluluk yok yani.

# : Template argümanının bir pointer türü olması durumunda bu partial specialization kullanılacak.



Partial Specialization Neler Olabilir?
--------------------------------------

Yukarıdaki koda göre örnek verelim:

	.
	..
	...

	template <typename T>
	struct Myclass<T&> {
		Myclass()
		{

		}
	};

Böyle olsa L value reference olan template argümanları için partial spec. olacak.

	Myclass<int&>
	Myclass<double&>  vs.


Hatta şöyle bile yazabiliriz;

	template <typename T>
	struct Myclass<T[10]> {
		Myclass()
		{

		}
	};

Şimdi template elemanı eğer 10 elemanlı bir dizi türü ise bu spec. kullanılacak.


Başka örnekler;

	
	template <typename T>
	struct Myclass {
		Myclass()
		{
			std::cout << "primary template\n";
		}
	};

	template <typename T, typename U>
	struct Myclass<std::pair<T, U> {
		Myclass()
		{
			std::cout << "Partial Spec\n";
		}
	};

	int main()
	{
		Myclass<int> x;						//primary template
		Myclass<double> y;					//primary template
		Myclass<std::pair<int, long>> z;	//partial spec.
	}


/////////////////////////////


	template <typename T, typename U>
	struct Myclass {
		Myclass()
		{
			std::cout << "primary template\n";
		}
	};

	template <typename T>	// #
	struct Myclass<T, T> {
		Myclass()
		{
			std::cout << "partial spec.\n";
		}
	};

	int main()
	{
		Myclass<int, double> m1;		//primary template
		Myclass<double, double> m2;		//partial spec.
	}

# : Eğer her iki template argümanı aynı türdense o zaman bu spec. kullanılacak.



Şöyle de yapabiliriz;

	template <typename T, typename U>
	struct Myclass {
		Myclass()
		{
			std::cout << "primary template\n";
		}
	};

	template <typename T>
	struct Myclass<T, int> {
		Myclass()
		{
			std::cout << "partial spec\n";
		}
	};

	int main()
	{
		Myclass<double, long> m1;	//primary template
		Myclass<double, int> m2;	//partial spec.
	}



böyle böyle yapabiliriz..


Kullanım senaryolarını anlayalım:

-type_traits kütüphanesi

















