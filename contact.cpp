#include <QAbstractListModel>

class Contact{
public:
    Contact(){}
    Contact(const QString &contactId,const QString &name="", const QString &phoneNumber=""){
        this->contactId = contactId;
        this->name = name;
        this->phoneNumber = phoneNumber;
    }
    QString getContactId() const{
        return contactId;
    }
    void setContactId(const QString &newContactId){
        contactId = newContactId;
    }

    QString getName() const{
        return name;
    }
    void setName(const QString &newName){
        name = newName;
    }

    QString getPhoneNumber() const{
        return phoneNumber;
    }
    void setPhoneNumber(const QString &newPhoneNumber){
        phoneNumber = newPhoneNumber;
    }

    bool operator== (const Contact& contact) const{
        return (this->contactId == contact.contactId);
    }

private:
    QString contactId;
    QString name;
    QString phoneNumber;
};
