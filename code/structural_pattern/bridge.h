#pragma once
#include<iostream>
#include<memory>
#include<string>
//通知系统*发送渠道 桥接示例
class Sender //发送基类
{
public:
    ~Sender() = default;
    virtual void send_way() = 0;
};

class Notification //通知基类
{
public:

    ~Notification() = default;
    virtual void notify() = 0;
    void reset_sender(std::shared_ptr<Sender> &sender)
    {   
        this->m_sender = sender;
    }
protected: //让子类能继承，应该是protected
    std::shared_ptr<Sender> m_sender; //组合：应该使用shared_ptr而非独占
    explicit Notification(std::shared_ptr<Sender> &sender)
    : m_sender(sender)
    {} //⭐基类虚构函数设置为protected:保证外部无法实例化，子类能正常初始化。
    //explicit防止隐式调用构造函数的情况发生（⭐父类指针指向子类对象和这里无关）
};



class AlertNotification : public Notification //紧急通告
{
public: 
    AlertNotification(std::shared_ptr<Sender> &sender)
    : Notification(sender)
    {}
    void notify() override
    {
        printf("it's a alert_notification!\n");
        this->m_sender->send_way();        
    }
};

class ReminderNotification : public Notification
{
public: 
    ReminderNotification(std::shared_ptr<Sender> &sender)
    : Notification(sender)
    {}
    void notify() override
    {
        printf("it's a reminder_notification\n");
        this->m_sender->send_way();
    }
};

class EmailSender : public Sender
{
public:
    void send_way() override
    {
        printf("send by email.\n");
    }
};

class SmsSender : public Sender
{
public:
    void send_way() override
    {
        printf("send by sms.\n");
    }
};

class PushSender : public Sender
{
public:
    void send_way() override
    {
        printf("send by push.\n");
    }
};

void bridge_test()
{
    std::shared_ptr<Sender> email_sender = std::make_shared<EmailSender>();
    std::shared_ptr<Sender> sms_sender = std::make_shared<SmsSender>();
    std::shared_ptr<Sender> push_sender = std::make_shared<PushSender>();

    AlertNotification an(email_sender);
    ReminderNotification rn(email_sender);
    an.notify();
    rn.notify();    

    an.reset_sender(sms_sender);
    rn.reset_sender(sms_sender);
    an.notify();
    rn.notify();

    an.reset_sender(push_sender);
    rn.reset_sender(push_sender);
    an.notify();
    rn.notify();
}
