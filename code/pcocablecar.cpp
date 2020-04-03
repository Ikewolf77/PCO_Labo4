//    ___  _________    ___  ___  ___  ___  //
//   / _ \/ ___/ __ \  |_  |/ _ \|_  |/ _ \ //
//  / ___/ /__/ /_/ / / __// // / __// // / //
// /_/   \___/\____/ /____/\___/____/\___/  //
//                                          //
// Auteurs : Mattei Simon, Kot Chau Ying

#include "pcocablecar.h"
#include <pcosynchro/pcothread.h>

#include <QDebug>
#include <QRandomGenerator>
constexpr unsigned int MIN_SECONDS_DELAY = 1;
constexpr unsigned int MAX_SECONDS_DELAY = 5;
constexpr unsigned int SECOND_IN_MICROSECONDS = 1000000;

// A vous de remplir les méthodes ci-dessous

PcoCableCar::PcoCableCar(const unsigned int capacity) : capacity(capacity)
{

}

PcoCableCar::~PcoCableCar()
{

}

void PcoCableCar::waitForCableCar(int id)
{
    nbSkiersWaitingSem.acquire();
    ++nbSkiersWaiting;
    nbSkiersWaitingSem.release();
}

void PcoCableCar::waitInsideCableCar(int id)
{
    qDebug() << "Skieu" << id << "est dans la télécabine";
}

void PcoCableCar::goIn(int id)
{
    waitOutsideCableCarSem.acquire();
}

void PcoCableCar::goOut(int id)
{
    waitInsideCalbeCarSem.acquire();
}

bool PcoCableCar::isInService()
{
    return inService;
}

void PcoCableCar::endService()
{
    inService = false;
}

void PcoCableCar::goUp()
{
    qDebug() << "La télécabine monte";
    PcoThread::usleep((MIN_SECONDS_DELAY + QRandomGenerator::system()->bounded(MAX_SECONDS_DELAY + 1)) * SECOND_IN_MICROSECONDS);
}

void PcoCableCar::goDown()
{
    qDebug() << "La télécabine descend";
    PcoThread::usleep((MIN_SECONDS_DELAY + QRandomGenerator::system()->bounded(MAX_SECONDS_DELAY + 1)) * SECOND_IN_MICROSECONDS);
}

void PcoCableCar::loadSkiers()
{
    nbSkiersWaitingSem.acquire();
    for(unsigned i = 0; i < nbSkiersWaiting && i < capacity; ++i){
        waitOutsideCableCarSem.release();
        --nbSkiersWaiting;
        ++nbSkiersInside;
    }
    nbSkiersWaitingSem.release();
}

void PcoCableCar::unloadSkiers()
{
    for(unsigned i = 0; i < nbSkiersInside; ++i)
        waitInsideCalbeCarSem.release();

    nbSkiersInside = 0;
}
