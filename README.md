QOrmSql
=======

Qt Object-Relational Mapping (ORM) library for MySQL and SQLite

Usage Examples
-------

In order to create model, you have to:

* Use `MyModel : public QOrmUserModel<MyModel>` pattern
* Create constructor with Q_INVOKABLE attribute and `QObject *` parameter
* Use either QOrmPublic or QOrmPrivate to define class properties
* Re-implement virtual methods `tableName()` and `accept(QOrmModelVisitor &visitor)`

Example with private fields:
```c++
class AndroidDevice : public QOrmUserModel<AndroidDevice>
{
    Q_OBJECT
public:
    Q_INVOKABLE AndroidDevice(QObject *parent);
    static QList<AndroidDevice *> findByName(const QString &name, QOrmDatabase *database = NULL);

    QString name() const;
    QString androidVersion() const;

    void setName(const QString &name);
    void setAndroidVersion(const QString &androidVersion);

protected:
    QString tableName() const;
    void accept(QOrmModelVisitor &visitor);

private:
    QOrmPrivate<QString> m_name;
    QOrmPrivate<QString> m_androidVersion;
};
```

Example with public fields:
```c++
class AndroidDevice : public QOrmUserModel<AndroidDevice>
{
    Q_OBJECT
public:
    Q_INVOKABLE AndroidDevice(QObject *parent);
    static QList<AndroidDevice *> findByName(const QString &name, QOrmDatabase *database = NULL);

    QOrmPublic<QString> name;
    QOrmPublic<QString> androidVersion;

protected:
    QString tableName() const;
    void accept(QOrmModelVisitor &visitor);
};
```
