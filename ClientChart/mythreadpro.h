#ifndef MYTHREADPRO_H
#define MYTHREADPRO_H

#include <QObject>

class myThreadpro : public QObject
{
    Q_OBJECT
public:
    explicit myThreadpro(QObject *parent = nullptr);

signals:

};

#endif // MYTHREADPRO_H
