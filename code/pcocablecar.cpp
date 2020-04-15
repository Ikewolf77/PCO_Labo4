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

PcoCableCar::PcoCableCar(const unsigned int capacity) : capacity(capacity), fifo(PcoSemaphore(capacity)), mutex(PcoSemaphore(1)),
                                                        waitInside(PcoSemaphore(0)), waitOutside(PcoSemaphore(0)), waitSkiers((0))
{

}

PcoCableCar::~PcoCableCar()
{

}

void PcoCableCar::waitForCableCar(int id)
{
    //se met dans la queue
    mutex.acquire();
    ++nbSkiersInQueue;
    mutex.release();
    fifo.acquire();

    //set met dans le prochain groupe de personne qui prend la télécabine
    mutex.acquire();
    if(inService){ //Gère la terminaison
        ++nbSkiersWaiting;
        --nbSkiersInQueue;
        mutex.release();
        qDebug() << "Skieur" << id << "attend la télécabine";
        waitOutside.acquire(); //Attend le CableCar
    } else {
        mutex.release();
    }
}

void PcoCableCar::waitInsideCableCar(int id)
{
    qDebug() << "Skieur" << id << "est dans la télécabine";
    waitSkiers.release(); //dit à la télécabine que le skieur est dedans
    waitInside.acquire(); //attend l'ouverture des portes pour descendre
}

void PcoCableCar::goIn(int id)
{
    qDebug() << "Skieur" << id << "monte dans la télécabine";
    fifo.release(); //on peut faire attendre une autre personne
}

void PcoCableCar::goOut(int id)
{
    qDebug() << "Skieur" << id << "sort dans la télécabine";
    waitSkiers.release(); //dit à la télécabine que je suis sortit
}

bool PcoCableCar::isInService()
{
    return inService;
}

void PcoCableCar::endService()
{
    mutex.acquire();

    inService = false;

    //relache les gens qui attendent à la fin du service
    for(unsigned i = 0; i < nbSkiersWaiting; ++i)
        waitOutside.release();

    for(unsigned i = 0; i < nbSkiersInQueue; ++i)
        fifo.release();

    mutex.release();
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
    mutex.acquire();

    qDebug() << "La télécabine embarque" << nbSkiersWaiting << "skieurs";

    //on fait en 2 fois, histoire d'éviter d'attendre les skieurs 1 par 1
    for(unsigned i = 0; i < nbSkiersWaiting; ++i){
        waitOutside.release();
    }

    for(unsigned i = 0; i < nbSkiersWaiting; ++i){
        waitSkiers.acquire();
        ++nbSkiersInside;
    }

    nbSkiersWaiting = 0;
    mutex.release();
}

void PcoCableCar::unloadSkiers()
{

    qDebug() << "La télécabine débarque les skieurs";

    //on fait en 2 fois, histoire d'éviter d'attendre les skieurs 1 par 1
    for(unsigned i = 0; i < nbSkiersInside; ++i)
        waitInside.release();

    for(unsigned i = 0; i < nbSkiersInside; ++i)
        waitSkiers.acquire();

    nbSkiersInside = 0;
}
