#ifndef CONTACTSMODEL_H
#define CONTACTSMODEL_H

#include <QAbstractListModel>
#include "contact.cpp"

class ContactsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit ContactsModel(QObject *parent = nullptr);

    enum{
        NameRole = Qt::UserRole,
        PhoneNumberRole
    };

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    virtual QHash<int, QByteArray> roleNames() const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    void populate(std::list<Contact> contacts);
    void updateDeletedContacts(std::list<Contact> contacts);

    QList<Contact> contactsList;

private:
    int getInsertIndex(QList<Contact> &contactsList, const Contact &contact);
};

#endif // CONTACTSMODEL_H
