#pragma once
#include<iostream>
#include<string>

using namespace std;

#if 0 //factory_pattern 工厂模式和简单工厂模式
class Animal
{
public:
    Animal(string name) : m_name(name){}
    virtual ~Animal(){} //析构函数定义成虚函数，因为父类不一定成析构子类的所有成员
    void print_info()
    {
        cout <<  "name:" << m_name << endl;
    }
    virtual void print_special() = 0;
protected://子类也有的属性，用protected
    string m_name;
};

class Fish : public Animal
{
public: 
    Fish(string name, size_t swim_velocity, int ID = 0) 
    : m_swim_velocity(swim_velocity)
    , Animal(name) 
    , m_ID(ID)
    {}
    ~Fish(){} //重载：如果子类有额外定义的内存，父类析构函数是无法析构的
    void print_special() override //重载父类虚函数
    {
        cout << "swim_velocity:" << m_swim_velocity 
        << " ID:" << m_ID << endl;
    }
private://没有多态，用private
    size_t m_swim_velocity;
    int m_ID;
};

class Dog : public Animal
{
public: 
    Dog(string name, size_t run_velocity, Dog *friend_ = nullptr) 
    : m_run_velocity(run_velocity)
    , m_friend(friend_)
    , Animal(name) 
    {}
    ~Dog()  //重载：如果子类有额外定义的内存，父类析构函数是无法析构的
    {
        delete m_friend;
        m_friend = nullptr;
    } 
    void print_special() override //重载父类虚函数
    {
        cout << "run_velocity:" << m_run_velocity 
        << " friend:" <<  (this->m_friend == nullptr ? "a lonly dog" : this->m_friend->m_name) <<  endl;
    }
private://没有多态，用private
    size_t m_run_velocity;
    Dog *m_friend;
};

//simple_factory简单工厂
class SimpleFactory
{
public:
    //枚举应该是public，这样才能让外部决定生产什么对象
    enum class Type
    {
        FISH,
        DOG
    };
    //这里使用static是因为这是一个无状态工厂，没有成员变量，逻辑简单，不需要创建对象
    static Animal* get_animal(Type type, string name,size_t velocity) //子类成员变量结构类似：使用枚举匹配
    {
        switch(type)
        {
            case Type::DOG:
            return new Dog(name, velocity);
            case Type::FISH:
            return new Fish(name, velocity);
            default:
            return nullptr;
        }
    }

    //子类成员变量结构不同：单独设计函数重载
    static Fish* create_fish(string name, size_t velocity, int ID)
    {
        return new Fish(name, velocity, ID);
    }

    static Dog* create_dog(string name, size_t velocity, Dog *friend_)
    {
        return new Dog(name, velocity, friend_);
    }
};

//factory 工厂
class Factory
{
public:
    Factory(size_t times = 0) : m_times(times) {}
    ~Factory() {printf("调用factory析构函数\n");}
    virtual Animal* basic_create(string name, int velocity) = 0;
    size_t get_times(){return this->m_times;}
private:
    size_t m_times;
};

//不使用static：记录工厂生产次数，需要创建对象
class FishFactory : public Factory
{
public:
    FishFactory(size_t times = 0) : Factory(times) {}
    ~FishFactory(){printf("调用fish factory析构函数\n");}
    Animal* basic_create(string name, int velocity) override
    {
        ++this->m_times;
        return new Fish(name, velocity);
    }
    
    Fish* fish_create(string name, int velocity, int ID)
    {
        ++this->m_times;
        return new Fish(name, velocity, ID);
    }
};

class DogFactory : public Factory
{
public:
    DogFactory(size_t times = 0) : Factory(times) {}
    ~DogFactory(){printf("调用dog factory析构函数\n");}
    Animal* basic_create(string name, int velocity) override
    {
        ++this->m_times;
        return new Dog(name, velocity);
    }
    
    Dog* dog_create(string name, int velocity, Dog *friend_)
    {
        ++this->m_times;
        return new Dog(name, velocity, friend_);
    }
};


void simple_factory_test()
{
    Animal *a1 = SimpleFactory::get_animal(SimpleFactory::Type::FISH, "default_fish", 10);
    Animal *a2 = SimpleFactory::get_animal(SimpleFactory::Type::DOG, "default_dog", 20);
    Animal *a3 = SimpleFactory::create_fish("fish!", 30, 100);
    Animal *a4 = SimpleFactory::create_dog("dog!", 40, static_cast<Dog*>(a2));

    a1->print_info();
    a1->print_special();
    a2->print_info();
    a2->print_special();
    a3->print_info();
    a3->print_special();
    a4->print_info();
    a4->print_special();
}


void factory_test()
{
    // 创建工厂
    DogFactory *dog_fac = new DogFactory(1);
    Factory *fish_fac = new FishFactory(2);

    // 使用工厂创建对象（模仿 simple_factory_test）
    Animal *a1 = fish_fac->basic_create("default_fish", 10);
    Animal *a2 = dog_fac->basic_create("default_dog", 20);
    Fish   *a3 = static_cast<FishFactory*>(fish_fac)->fish_create("fish!", 30, 100);

    // 为避免双重删除，单独 new 一个 friend 给 a4，让 a4 在析构时负责释放
    Dog *friend_dog = new Dog("pal", 5, nullptr);
    Dog *a4 = dog_fac->dog_create("dog!", 40, friend_dog);

    // 打印信息
    a1->print_info(); a1->print_special();
    a2->print_info(); a2->print_special();
    a3->print_info(); a3->print_special();
    a4->print_info(); a4->print_special();

    // 释放资源（注意 a4 的析构函数会 delete friend_dog）
    delete a1;
    delete a2;
    delete a3;
    delete a4;

    // 释放工厂
    delete dog_fac;
    delete fish_fac;
}

#endif

#if 1 //abstract_factory_pattern 抽象工厂模式
class Shoes
{
public:
    Shoes(){printf("it's a pair of ");}
    virtual ~Shoes(){} // 添加虚析构，启用多态和 dynamic_cast
};

class Clothes
{
public:
    Clothes(){printf("look at this ");}
    virtual ~Clothes(){} // 添加虚析构
};

class BlackShoes : public Shoes
{
public:
    BlackShoes(){printf("black shoes!\n");}
};

class RedShoes : public Shoes
{
public:
    RedShoes(){printf("red shoes!\n");}
};

class BlackClothes : public Clothes
{
public: 
    BlackClothes(){printf("black clothes!\n");}
};

class RedClothes : public Clothes
{
public: 
    RedClothes(){printf("red clothes!\n");}
};

//抽象工厂：生产的不是单个对象而是多个属于同一主题的对象->提高兼容性
class AbstractFactory
{
public:
    virtual Shoes* create_shoes() = 0;
    virtual Clothes* create_clothes() = 0;
};

class RedFactory : public AbstractFactory
{
public:
    Shoes* create_shoes() {return new RedShoes;}
    Clothes *create_clothes() {return new RedClothes;}
};

class BlackFactory : public AbstractFactory
{
public:
    Shoes* create_shoes() {return new BlackShoes;}
    Clothes *create_clothes() {return new BlackClothes;}
};

void abstract_factory_test()
{
    // 使用抽象工厂指针创建具体工厂
    AbstractFactory* red_f = new RedFactory;
    AbstractFactory* black_f = new BlackFactory;

    // 通过工厂创建一组同主题产品（多态使用）
    Shoes* r_shoes = red_f->create_shoes();
    Clothes* r_clothes = red_f->create_clothes();

    Shoes* b_shoes = black_f->create_shoes();
    Clothes* b_clothes = black_f->create_clothes();

    // 首选：通过基类的虚接口使用对象（如果需要，给基类增加虚方法）
    // 示例：只是打印，构造时已经输出了信息

    // 若确实需要访问子类特有接口，使用 dynamic_cast（安全）
    if (RedClothes* rc = dynamic_cast<RedClothes*>(r_clothes)) {
        // 可以访问 RedClothes 的特有方法/数据（示例中无额外成员）
        (void)rc;
    }

    // 清理
    delete r_shoes;
    delete r_clothes;
    delete b_shoes;
    delete b_clothes;
    delete red_f;
    delete black_f;
}

#endif