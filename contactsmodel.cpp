#include "contactsmodel.h"
#include <QDebug>
#include <QGuiApplication>
#include <QJniObject>
#include <QJsonDocument>

ContactsModel *m_instance = nullptr;

ContactsModel::ContactsModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_instance = this;
    auto activity = QJniObject(QNativeInterface::QAndroidApplication::context());
    activity.callMethod<void>("getContacts");
}

int ContactsModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    return contactsList.size();
}

QVariant ContactsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch(role){
    case NameRole:
        return QVariant(contactsList[index.row()].getName());
    case PhoneNumberRole:
        return QVariant(contactsList[index.row()].getPhoneNumber());
    }
    return QVariant();
}

QHash<int, QByteArray> ContactsModel::roleNames() const
{
    QHash<int, QByteArray> roleNames;
    roleNames[NameRole] = "name";
    roleNames[PhoneNumberRole] = "phoneNumber";
    return roleNames;
}

bool ContactsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid()) {
        if (data(index, role) != value) {
            switch(role){
                case NameRole:
                    contactsList[index.row()].setName(value.toString());
                case PhoneNumberRole:
                    contactsList[index.row()].setPhoneNumber(value.toString());
            }
            emit dataChanged(index, index, {role});
            return true;
        }
    }
    return false;
}

void ContactsModel::populate(std::list<Contact> contacts)
{
    if(contactsList.isEmpty()){
        beginResetModel();
        for(const Contact &contact : contacts){
            contactsList.append(contact);
        }
        std::sort(contactsList.begin(), contactsList.end(), [](const Contact& contact1, const Contact& contact2) {
            auto nameDiff = contact1.getName().toLower().compare(contact2.getName().toLower());
            if(nameDiff == 0)
                return contact1.getContactId() < contact2.getContactId();
            else
                return nameDiff < 0;
        });
        endResetModel();
    }else{
        for(Contact &contact : contacts) {
            QList<Contact>::iterator curItr = std::find(contactsList.begin(), contactsList.end(), contact);
            qDebug() << "contact.contactId = " << contact.getContactId();

            const auto elemRowIndex = std::distance(contactsList.begin(), curItr);
            if(curItr != contactsList.end()){
                qDebug() << "existing contact edited at "<<elemRowIndex;
                QModelIndex qModelIndex = index(elemRowIndex, 0, qModelIndex);
                setData(qModelIndex, QVariant(contact.getName()), NameRole);
                setData(qModelIndex, QVariant(contact.getPhoneNumber()), PhoneNumberRole);
            }else{
                QPair<int, QList<Contact>::iterator> qPair = getInsertIndex(contactsList, contact);
                if(qPair.first != -1) {
                    qDebug() << "New contact added case, insert index = "<<qPair.first;
                    beginInsertRows(QModelIndex(), qPair.first, qPair.first);
                    contactsList.insert(qPair.second, contact);
                    endInsertRows();
                }
            }
        }
    }
}

QPair<int, QList<Contact>::iterator> ContactsModel::getInsertIndex(QList<Contact> &contactsList, const Contact &contact) {
    if(contact.getName().isEmpty())
        return qMakePair(-1, contactsList.begin());

    QList<Contact>::iterator itr = contactsList.begin();
    QString name = contact.getName().toLower();
    //qDebug() << "searching to insert name "<<name;
    int insertIndex = 0;

    for (; itr != contactsList.end(); ++itr, ++insertIndex) {
        bool insertPosFound = false;
        int startCharIndex = 0, searchStartCharIndex = 0;
        int endCharIndex = itr->getName().length(), searchEndCharIndex = name.length();
        QString curItrName = itr->getName().toLower();

        while (startCharIndex < endCharIndex && searchStartCharIndex < searchEndCharIndex) {
            QChar startChar = curItrName[startCharIndex];
            QChar searchStartChar = name[searchStartCharIndex];
            if (startChar == searchStartChar) {
                startCharIndex++;
                searchStartCharIndex++;
            }else if (startChar > searchStartChar) {
                insertPosFound = true;
                break;
            } else {
                break;
            }
        }
        if (insertPosFound || (startCharIndex < endCharIndex && searchStartCharIndex == searchEndCharIndex)) {
            break;
        }
    }
    return qMakePair(insertIndex, itr);
}

void ContactsModel::updateDeletedContacts(std::list<Contact> contacts)
{
    for(Contact &contact : contacts) {
        QList<Contact>::iterator curItr = std::find(contactsList.begin(), contactsList.end(), contact);
        const auto elemRowIndex = std::distance(contactsList.begin(), curItr);
        if(elemRowIndex < contactsList.size()){
            qDebug() << "contact.contactId = "<< contact.getContactId()<<" existing contact deleted at "<<elemRowIndex;
            beginRemoveRows(QModelIndex(), elemRowIndex, elemRowIndex);
            contactsList.removeAt(elemRowIndex);
            endRemoveRows();
        }
    }
}

extern "C" JNIEXPORT void JNICALL Java_com_test_ContactsActivity_onModifiedContactsReceived(JNIEnv* env, jobject obj, jstring contactsJsonStr) {

    QVariantList qJsonDoc = QJsonDocument::fromJson(env->GetStringUTFChars(contactsJsonStr,0)).toVariant().toList();
    std::list<Contact> contacts;
    QVariantList::iterator it;
    for(it = qJsonDoc.begin(); it != qJsonDoc.end(); ++it)
    {
        QVariantMap contactMap = (*it).toMap();
        QString contactId = contactMap["contactID"].toString();
        QString   name= contactMap["name"].toString();
        QString    phoneNumber = contactMap["phoneNumber"].toString();
        contacts.push_back(Contact(contactId, name, phoneNumber));
    }
    m_instance->populate(contacts);
}

extern "C" JNIEXPORT void JNICALL Java_com_test_ContactsActivity_onDeletedContactsReceived(JNIEnv* env, jobject obj, jstring contactsJsonStr) {

    QVariantList qJsonDoc = QJsonDocument::fromJson(env->GetStringUTFChars(contactsJsonStr,0)).toVariant().toList();
    std::list<Contact> contacts;
    QVariantList::iterator it;
    for(it = qJsonDoc.begin(); it != qJsonDoc.end(); ++it)
    {
        QVariantMap contactMap = (*it).toMap();
        QString contactId = contactMap["contactID"].toString();
        contacts.push_back(Contact(contactId));
    }
    m_instance->updateDeletedContacts(contacts);
}
