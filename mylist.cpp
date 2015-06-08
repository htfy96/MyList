#ifndef _MYLIST_HPP
#define _MYLIST_HPP

/******************************
 *  Prohibit g++ --std=c++11
 ******************************/
#ifdef __GNUC__ 
#ifndef __clang__
#if __cplusplus>=201103L
#error "Since g++ has bug in the scope of 'this'(https://gcc.gnu.org/bugzilla/show_bug.cgi?id=52869), g++ with c++11 standard is incompatible with this program"
#endif
#endif
#endif

#include <iostream>

/*************************************************
 * General macros for noexcept or throw feature
 * Dirty tricks - disgusting
 * ***************************8*******************/
#if __cplusplus >= 201103L
#define __MYLIST_NOEXCEPT() noexcept
#define __MYLIST_EXCEPT_1(a_) noexcept(false)
#define __MYLIST_EXCEPT_2(a_,b_) noexcept(false)
#define __MYLIST_EXCEPT_ANY() noexcept(false)
#define __MYLIST_NOEXCEPT_IF(a_) noexcept( noexcept((a_)) )
#define __MYLIST_NOEXCEPT_IF_2(a_,b_) noexcept(noexcept(a_) && noexcept(b_))
#define __MYLIST_NOEXCEPT_IF_3(a_,b_,c_) noexcept( noexcept((a_)) && noexcept((b_)) && noexcept((c_)))
#define __MYLIST_COPY T( T() )
#define __MYLIST_CONSTRUCT T()
#define __MYLIST_ASSIGN (T() = T())
#define __MYLIST_NEW new T[1]
#else
#define __MYLIST_NOEXCEPT() throw()
#define __MYLIST_EXCEPT_1(a_) throw(a_)
#define __MYLIST_EXCEPT_2(a_,b_) throw(a_,b_)
#define __MYLIST_NOEXCEPT_IF(a_) 
#define __MYLIST_NOEXCEPT_IF_2(a_,b_) 
#define __MYLIST_NOEXCEPT_IF_3(a_,b_,c_)
#define __MYLIST_EXCEPT_ANY() 
#endif

/**********************************************************
 * Exception classes
 *
 * MyListBaseException (exceptionType, lineno, filename, msg, print())
 * |
 * +----MyListLogicError
 *      |
 *      +----MyListPopFromNullError
 *      +----MyListOutOfRangeError
 *      +----MyListResizeToZeroError
 *
 * Due to the lack of meta data in c++98, extra tag "exceptionType"
 * is included in an object
 * *******************************************************/

#define __EXCEPTION_PREFIX __LINE__,__FILE__,__func__
//OK, make them happy
//Duplicate code sucks
#define __MYLIST_EXCEPTION_CONS(thisClass) thisClass(int lineno_, const char* filename_, const char* func_, const char* msg_):
#define __MYLIST_EXCEPTION_CONS2(baseClass,ThisClass) baseClass(lineno_,filename_,func_,msg_),exceptionType(#ThisClass) {}
#define __MYLIST_EXCEPTION_DEF_CONS(thisClass) thisClass():exceptionType(#thisClass){}

struct MyListBaseException {
    public:
        const char* const exceptionType;
        int lineno;
        const char* filename;
        const char* func;
        const char* msg;
        __MYLIST_EXCEPTION_CONS(MyListBaseException)
            exceptionType("MyListBaseException"),lineno(lineno_),filename(filename_),func(func_),msg(msg_) {}
        __MYLIST_EXCEPTION_DEF_CONS(MyListBaseException)
            virtual ~MyListBaseException() {};
        virtual void print()
        {
            std::cout<<exceptionType<<": In "<<filename<<" at line "<<lineno<<" in function "<<func<<"  ["<<msg<<"]"<<std::endl;
        }
};
struct MyListLogicError:MyListBaseException {
    const char* const exceptionType;
    __MYLIST_EXCEPTION_CONS(MyListLogicError)
        __MYLIST_EXCEPTION_CONS2(MyListBaseException,MyListLogicError)
        __MYLIST_EXCEPTION_DEF_CONS(MyListLogicError)
};
struct MyListPopFromNullError:MyListLogicError{
    const char* const exceptionType;
    __MYLIST_EXCEPTION_CONS(MyListPopFromNullError)
        __MYLIST_EXCEPTION_CONS2(MyListLogicError,MyListPopFromNullError)
        __MYLIST_EXCEPTION_DEF_CONS(MyListPopFromNullError)
};
struct MyListOutOfRangeError:MyListLogicError{
    const char* const exceptionType;
    __MYLIST_EXCEPTION_CONS(MyListOutOfRangeError)
        __MYLIST_EXCEPTION_CONS2(MyListLogicError,MyListOutOfRangeError)
        __MYLIST_EXCEPTION_DEF_CONS(MyListOutOfRangeError)
};
struct MyListResizeToMinusError:MyListLogicError{
    const char* const exceptionType;
    __MYLIST_EXCEPTION_CONS(MyListResizeToMinusError)
        __MYLIST_EXCEPTION_CONS2(MyListLogicError,MyListResizeToMinusError)
        __MYLIST_EXCEPTION_DEF_CONS(MyListResizeToMinusError)
};

//Forward Declaration
template<class T>
class MyList;

template<typename U>
MyList<U> operator +(const MyList<U> &l1, const MyList<U> &l2);

template<typename U>
std::ostream & operator << (std::ostream &os, const MyList<U> &obj) __MYLIST_NOEXCEPT();

template<typename T>
MyList<T> operator + (const MyList<T> &l1, const T &item);

//Declaration of MyList<T>
template<class T>
class MyList{
    private:
        T *a;
        int capacity;
        int size;
        void double_space(int targetSize=-1) __MYLIST_NOEXCEPT_IF_3(throw std::bad_alloc(),throw MyListResizeToMinusError(), __MYLIST_COPY);
        template<bool less> void sort_impl(T* l, T* r) __MYLIST_NOEXCEPT_IF_2(__MYLIST_COPY, __MYLIST_CONSTRUCT);
    public:

        MyList() __MYLIST_NOEXCEPT() : a(NULL), capacity(0), size(0){};
        MyList(int num, const T &item) __MYLIST_NOEXCEPT_IF_2(throw std::bad_alloc(), __MYLIST_COPY);//将item重复num次填入数组中。
        MyList(const MyList &l)  __MYLIST_NOEXCEPT_IF_2(throw std::bad_alloc(), __MYLIST_COPY);//深复制另外一个MyList。
        MyList(T* arr, int len)  __MYLIST_NOEXCEPT_IF_2(throw std::bad_alloc(), __MYLIST_COPY);//以arr的前len个元素构造数组

        void push(const T &item) __MYLIST_NOEXCEPT_IF_2(double_space(), __MYLIST_COPY)
            ;//将item添加在MyList最后。
        T pop() __MYLIST_NOEXCEPT_IF_2(throw MyListPopFromNullError(), __MYLIST_COPY());//将MyList中最后一个元素删除，并返回这个删除的元素。
        void insert(int index, const T &item) __MYLIST_NOEXCEPT_IF_2(double_space(), __MYLIST_COPY);//将item插入到place处。
        void clean() __MYLIST_NOEXCEPT();//清空数组。
        int get_size() const __MYLIST_NOEXCEPT();//返回MyList中元素的数量。
        void erase(int start, int end) __MYLIST_NOEXCEPT_IF_2(throw MyListOutOfRangeError(), __MYLIST_COPY); //删除MyList中第start到第end位的元素，包括两边。
        //T get_item(int index) const;//返回第index个元素。

        MyList<T> get_item(int start, int end) const __MYLIST_NOEXCEPT_IF_2(throw MyListOutOfRangeError(), MyList<T>(a,0));//返回MyList中第start到第end位的元素，包括两边。此处需要像python一样接受负数，具体见测试代码。
        T get_item(int index) const __MYLIST_NOEXCEPT_IF_2(throw std::bad_alloc(), __MYLIST_COPY );
        int count(const T &item) const __MYLIST_NOEXCEPT_IF(item == a[0]);//返回MyList中和item相等的元素的个数。
        void remove(const T &item) __MYLIST_NOEXCEPT_IF(erase(0,0));//删除MyList中第一个和item相等的元素。


        friend MyList operator + <> (const MyList<T> &l1, const MyList<T> &l2); 
        //合并两个MyList
        friend MyList operator + <> (const MyList &l1, const T &item); //同push(T item)，但不修改l1，返回一个新数组
        MyList &operator = (const MyList<T> &l) __MYLIST_NOEXCEPT_IF_2(double_space(), __MYLIST_COPY);//赋值
        MyList &operator += (const T &item) __MYLIST_NOEXCEPT_IF(push(T()));//同push(T item)
        MyList &operator += (const MyList<T> &l) __MYLIST_NOEXCEPT_IF_2(double_space(),*this+=T());//将一个MyList加入到本个MyList之后。
        T &operator [](int index) const  __MYLIST_NOEXCEPT_IF_2(throw MyListOutOfRangeError(), __MYLIST_COPY);//返回第index个元素。
        friend std::ostream & operator<< <>(std::ostream &os, const MyList<T> &obj) __MYLIST_NOEXCEPT();//如果T是可以直接cout的类的话（比如int），按Python数组的格式输出MyList中的每一个元素，例如：
        // [1, 2, 5, 4, 1]

        void sort(bool less=true) __MYLIST_NOEXCEPT_IF_2(__MYLIST_COPY, __MYLIST_CONSTRUCT)
            ;//实现一个快速排序或归并排序，对支持比较运算符（>=<）的类进行排序。
        // 若less=true，按照从小到大排列，否则按从大到小排列
        void reverse() __MYLIST_NOEXCEPT_IF_2(__MYLIST_COPY, __MYLIST_CONSTRUCT);//将MyList的元素倒过来。
        void swap(const MyList<T>& b) __MYLIST_NOEXCEPT();
        ~MyList(){delete [] a;}
};



int __max(int a, int b) { return (a>b)?a:b; }

//Strong Exception Safety
template<typename T>
void MyList<T>::double_space(int targetSize) __MYLIST_NOEXCEPT_IF_3(throw std::bad_alloc(),throw MyListResizeToMinusError(), __MYLIST_COPY)
{
    T* tmp;
    if (targetSize == -1) targetSize = __max(1,capacity*2);
    if (targetSize<0) throw MyListResizeToMinusError(__EXCEPTION_PREFIX, "can't resize to minus");
    tmp = new T [targetSize];
    try
    {
        for(int i=0;i<size;++i)
          tmp[i]=a[i];          //If exception raises, a will not be influenced
    } catch(...)
    {
        delete[] tmp;
        throw;
    }
    for (int i=0;i<size;++i)
      a[i].~T();
    delete[] a;
    capacity = targetSize;
    a = tmp;
}

//Strong Exception Safety (OK... there is no difference between Basic/Strong ES in Constructor)
template<typename T>
MyList<T>::MyList(int num, const T &item) __MYLIST_NOEXCEPT_IF_2(throw std::bad_alloc(), __MYLIST_COPY)
{
    a = new T [num];
    capacity = num;
    size = num;
    try
    {
        for(int i=0; i<num; ++i)
          a[i] = item;
    } catch (...)                   // Constructor will not call destructor after an exception is thrown, therefore manual delete[] a is required in case of memory leak
    {
        delete[] a;
        throw;
    }
}

//Strong Exception Safety
template<typename T>
MyList<T>::MyList(const MyList &l)  __MYLIST_NOEXCEPT_IF_2(throw std::bad_alloc(), __MYLIST_COPY)
{
    size = l.size;
    capacity = l.capacity;
    a = new T[capacity];

    try
    {
        for(int i=0;i<size;++i)
          a[i]=l.a[i];
    } catch(...)
    {
        delete[] a;
        throw;
    }
}

//Strong Exception Safety
template<typename T>
MyList<T>::MyList(T* arr, int len)  __MYLIST_NOEXCEPT_IF_2(throw std::bad_alloc(), __MYLIST_COPY)
{
    size = len;
    capacity = len;

    a=new T[capacity];

    try
    {
        for(int i=0;i<size;++i) //Out_of_range is not an exception
          a[i]=arr[i];
    } catch(...)
    {
        delete[] a;
        throw;
    }
}

//Strong Exception Safety (SES does not mean everything remains unchanged after exception, but means everything *exposed to user* remains unchanged)
template<typename T>
void MyList<T>::push(const T &item) __MYLIST_NOEXCEPT_IF_2(double_space(), __MYLIST_COPY)
{
    if (capacity == size)
      double_space();
    a[size]=item;       //if exception raises, size will remain unchanged. Since a[size] is undefined, then the part exposed to user (a[0]~a[size-1]) remains unchanged)
    ++size;
}

//Strong Exception Safety
template<typename T>
T MyList<T>::pop() __MYLIST_NOEXCEPT_IF_2(throw MyListPopFromNullError(), __MYLIST_COPY())
{
    if (!size)
      throw MyListPopFromNullError(__EXCEPTION_PREFIX, "can't pop from a null list");
    T tmp=a[size-1];
    a[--size].~T(); //Destructor never throws exception
    try
    {
        return tmp; //Copy constructor may throw exception! 
    } catch(...)
    {
        ++size; //Even if copy constructor throws an exception, size will still be decreased
        throw;
    }
}

//Weak Exception safety
template<typename T>
void MyList<T>::insert(int index, const T &item) __MYLIST_NOEXCEPT_IF_2(double_space(), __MYLIST_COPY)
{
    if (index<0 || index>size) throw MyListOutOfRangeError(__EXCEPTION_PREFIX, "index of insert out of range");
    if (size == capacity)
      double_space();
    ++size;
    for (int i=size-1; i>index; --i)
      a[i]=a[i-1];
    a[index]=item;
}

//No exception
template<typename T>
void MyList<T>::clean() __MYLIST_NOEXCEPT()
{
    for(int i=0;i<size;++i)
      a[i].~T();
    size=0;
}

//No exception
template<typename T>
int MyList<T>::get_size() const __MYLIST_NOEXCEPT()
{
    return size;
}

//Weak Exception Safety
template<typename T>
void MyList<T>::erase(int start, int end) __MYLIST_NOEXCEPT_IF_2(throw MyListOutOfRangeError(), __MYLIST_COPY)
{
    if (start<0 || start>=size || end<0 || end>=size || start>end)
      throw MyListOutOfRangeError(__EXCEPTION_PREFIX, "index of erase out of range");
    for (int i=start; i<=end; ++i)
      a[i].~T();
    for (int i=end+1; i<size; ++i)
      a[i-end+start-1] = a[i];      //error in copy -> correct destruct
    size -= end-start+1;
}

//Strong Exception Safety
template<typename T>
T MyList<T>::get_item(int index) const __MYLIST_NOEXCEPT_IF_2(throw std::bad_alloc(), __MYLIST_COPY) 
{
    if (index<0 || index>=size) 
      throw MyListOutOfRangeError(__EXCEPTION_PREFIX, "index of item out of range");
    return a[index];
}

//Strong Exception Safety
template<typename T>
MyList<T> MyList<T>::get_item(int start, int end) const __MYLIST_NOEXCEPT_IF_2(throw MyListOutOfRangeError(), MyList<T>(a,0))
{
    if (start<0) start+=size;
    if (end<0) end+=size;
    if (start<0 || start>=size || end<0 || end>=size) throw MyListOutOfRangeError();
    if (start>end) return MyList<T>();
    return MyList<T>(a+start, end-start+1); //RVO
}

//Strong Exception Safety
template<typename T>
int MyList<T>::count(const T &item) const __MYLIST_NOEXCEPT_IF(item == a[0])
{
    int ans=0;
    for(int i=0; i<size; ++i)
      ans+=(item == a[i]);
    return ans;
}

//Weak Exception Safety - because erase(i,i) has WES
template<typename T>
void MyList<T>::remove(const T &item) __MYLIST_NOEXCEPT_IF(erase(0,0))
{
    for (int i=0;i<size;++i)
      if (item == a[i])
      {
          erase(i,i);
          return;
      }
}

//Strong Exception Safety
template<typename T>
MyList<T>& MyList<T>::operator+=(const T &item) __MYLIST_NOEXCEPT_IF(push(T()))
{
    push(item);
    return *this;
}

//Weak Exception Safety
template<typename T>
MyList<T>& MyList<T>::operator +=(const MyList<T> &l) __MYLIST_NOEXCEPT_IF_2(double_space(),*this+=T())
{
    if (capacity < size + l.size)
      double_space(size + l.size);
    for (int i=0; i<l.size; ++i)
      (*this) += l[i];
    return *this;
}

//Weak Exception Safety
template<typename U>
MyList<U> operator +(const MyList<U> &l1, const MyList<U> &l2) 
{
    MyList<U> result(l1);
    return result+=l2;
}

template<typename T>
MyList<T> operator + (const MyList<T> &l1, const T &item){
    MyList<T> result(l1);
    l1.push(item);
    return l1;
}

//Weak Exception Safety
template<typename T>
MyList<T>& MyList<T>::operator= (const MyList<T> &l) __MYLIST_NOEXCEPT_IF_2(double_space(), __MYLIST_ASSIGN)
{
    if (capacity<l.size) double_space(l.size);
    for (int i=size;i<l.size;++i)
      a[i].~T();
    size = l.size;
    for(int i=0; i<l.size; ++i)
      a[i]=l[i];
    return *this;
}

//Strong Exception Safety
template<typename T>
T& MyList<T>::operator[](int index) const __MYLIST_NOEXCEPT_IF_2(throw MyListOutOfRangeError(), __MYLIST_COPY)
{
    if (index<0 || index>=size) 
      throw MyListOutOfRangeError(__EXCEPTION_PREFIX, "index out of range");
    return a[index];
}

//Strong Exception Safety
template<typename U>
std::ostream & operator << (std::ostream &os, const MyList<U> &obj) __MYLIST_NOEXCEPT()
{
    os<<'[';
    for (int i=0;i<obj.size;++i)
    {
        if (i) os<<", ";
        os<<obj[i];
    }
    os<<']';
    return os;
}

template<typename T, bool less>
inline bool __comp(const T& a, const T& b) { if (less) return a<b; return a>b; }
template<typename T> inline void __swap(T& a, T& b) { T y=a; a=b; b=y; }
const int SORT_THRESHOLD=32;

//Oops... Weak Exception Safety
template<typename T>
template<bool less>
void MyList<T>::sort_impl(T* l, T* r) __MYLIST_NOEXCEPT_IF_2(__MYLIST_COPY, __MYLIST_CONSTRUCT)
{

    if (l>=r) return;
    switch(r-l)
    {
        case 1: return;
        case 2:
                if(__comp<T,less>(*r,*l)) __swap(*l,*r);
                return;
        case 3:
                if (__comp<T,less>(*(l+1), *l)) __swap(*l, *(l+1));
                if (__comp<T,less>(*r, *(l+1))) __swap(*(l+1), *r);
                return;
        case 4:
                if (__comp<T,less>(*(l+1), *l)) __swap(*l, *(l+1));
                if (__comp<T,less>(*(l+2), *(l+1))) __swap(*(l+2), *(l+1));
                if (__comp<T,less>(*(l+2), *r)) __swap (*(l+2),*r);
                return;                
        default:
                T *i=l, *j=r;
                T &x= *(l+((r-l)>>1));
                if (r-l<SORT_THRESHOLD)
                {
                    for(i=l;i<r;++i)
                      for (j=i+1;j<r;++j)
                        if (__comp<T,less>(*j, *i))
                          __swap(*i, *j);
                    return;
                }
                do
                {
                    while (__comp<T,less>(*i,x)) ++i;
                    while (__comp<T,less>(x,*j)) --j;
                    if (i<=j)
                    {
                        __swap(*i, *j);
                        ++i;
                        --j;
                    }
                } while (i<=j);
                if (l<j) sort_impl<less>(l, j);
                if (i<r) sort_impl<less>(i, r);
    }
}

template<typename T>
void MyList<T>::sort(bool less) __MYLIST_NOEXCEPT_IF_2(__MYLIST_COPY, __MYLIST_CONSTRUCT)

{
    if(less) sort_impl<true>(a,a+size-1);
    else sort_impl<false>(a,a+size-1);
}

template<typename T>
void MyList<T>::reverse() __MYLIST_NOEXCEPT_IF_2(__MYLIST_COPY, __MYLIST_CONSTRUCT)
{
    T y;
    for(int i=0;i<size/2;++i)
    {
        y=a[i];
        a[i]=a[size-i-1];
        a[size-i-1]=y;
    }
}

template<typename T>
void MyList<T>::swap(const MyList<T>& b) __MYLIST_NOEXCEPT()
{
    int y=capacity;
    capacity=b.capacity;
    b.capacity=y;
    y=size;
    size=b.size;
    b.size=size;
    int * yy=a;
    a=b.a;
    b.a=yy;
}


#endif
using namespace std;

int main()
{	MyList<int> a, b;
    int i;
    for (i=0; i<5; ++i)
      a.push(i);
    // a = [0, 1, 2, 3, 4]
    cout<<a<<endl;
    a[3] = 15; // a = [0, 1, 2, 15, 4]
    cout<<a<<endl;
    a.sort(); // a = [0, 1, 2, 4, 15]
    cout<<a<<endl;
    a.reverse(); // a = [15, 4, 2, 1, 0]
    a += 12; // a = [15, 4, 2, 1, 0, 12]
    cout<<a<<endl;
    b = a.get_item(4, -3); // b = [] *若start > end，返回空数组
    b = a.get_item(3, -1); // b = [1, 0, 12] 
    a += b; // a = [15, 4, 2, 1, 0, 12, 1, 0, 12]

    for (i=0; i<a.get_size(); ++i)
      cout<<a.get_item(i)<<endl;
    cout<<a.count(5)<<endl;
    b.clean(); // b = []
    cout<<b.get_size()<<endl;
    a.erase(2, 6); // a = [15, 4, 0, 12]
    cout<<a<<endl;
    try
    {
        a.erase(3,9);
    } catch(MyListBaseException& e)
    {
        e.print();
    }
    // after exception nothing changes
    b = a + a; // b = [15, 4, 0, 12, 15, 4, 0, 12]
    b.insert(3, 116); // b = [15, 4, 0, 116, 12, 15, 4, 0, 12]
    b.remove(4); // b = [15, 0, 116, ...]
    cout<<b<<endl;
    MyList<double> c(10, 3.14);
    for (i=0; i<100; ++i)
      c.push(1.1*i);
    c.sort();
    cout<<c<<endl;
    c.sort(false);
    cout<<c.get_item(100, 105)<<endl;
    cout<<c<<endl;
    c.erase(2,98);
    cout<<c.get_size()<<endl;
    cout<<c<<endl;

    return 0;
}

