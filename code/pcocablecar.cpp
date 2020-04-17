//    ___  _________    ___  ___  ___  ___  //
//   / _ \/ ___/ __ \  |_  |/ _ \|_  |/ _ \ //
//  / ___/ /__/ /_/ / / __// // / __// // / //
// /_/   \___/\____/ /____/\___/____/\___/  //
//                                          //
// Auteurs : Prénom Nom, Prénom Nom

#include "pcocablecar.h"
#include <pcosynchro/pcothread.h>

#include <QDebug>
#include <QRandomGenerator>
#include <iostream>


constexpr unsigned int MIN_SECONDS_DELAY = 1;
constexpr unsigned int MAX_SECONDS_DELAY = 5;
constexpr unsigned int SECOND_IN_MICROSECONDS = 1000000;

// A vous de remplir les méthodes ci-dessous

PcoCableCar::PcoCableCar(const unsigned int capacity) : capacity(capacity),
                                                        inCabinMutex(1),
                                                        waitingMutex(1),
                                                        load(0),
                                                        canGo(0),
                                                        unload(0)
{
}

PcoCableCar::~PcoCableCar()
{
}

void PcoCableCar::waitForCableCar(int id)
{
    // Increment le nombre de skier en attente
    waitingMutex.acquire();
    nbSkiersWaiting++;
    waitingMutex.release();
    qDebug() << "Skier "<< id << " attend la cabine";


    // Attends que la cabine permet au skier de monter
    load.acquire();

}

void PcoCableCar::waitInsideCableCar(int id)
{
   qDebug() << "Skier " << id << " attend dans la cabine" ;

   // Indique qu'on à la cabine qu'un skier est monté
   canGo.release();

   // Attends le départ de la cabine
   unload.acquire();

}

void PcoCableCar::goIn(int id)
{

    qDebug() << "Skier " << id << " entre dans la cabine";

}

void PcoCableCar::goOut(int id)
{

    qDebug() << "Skier " << id << " sort de la cabine";

    // Indique qu'un skier est sorti
    canGo.release();
}

bool PcoCableCar::isInService()
{
    return inService;
}

void PcoCableCar::endService()
{
    inService = false;
    // Fin du server, on relache les personnes en attente
    for(unsigned i = 0; i < nbSkiersWaiting;++i){
        load.release();
    }
}

void PcoCableCar::goUp()
{
    qDebug() << "Le télécabine monte";
    PcoThread::usleep((MIN_SECONDS_DELAY + QRandomGenerator::system()->bounded(MAX_SECONDS_DELAY + 1)) * SECOND_IN_MICROSECONDS);
}

void PcoCableCar::goDown()
{
    qDebug() << "Le télécabine descend";
    PcoThread::usleep((MIN_SECONDS_DELAY + QRandomGenerator::system()->bounded(MAX_SECONDS_DELAY + 1)) * SECOND_IN_MICROSECONDS);
}

void PcoCableCar::loadSkiers()
{
    // Permet de load uniquement si c'est en service
    if(inService){
        qDebug() << "La télécabine charge les skiers";

        // Regarde le nombre de personne qui doit monté
        unsigned nbCanGo = nbSkiersWaiting < capacity ? nbSkiersInside : capacity;

        // Laisse les skier monter
        for(unsigned i = 0 ; i < nbCanGo; ++i){
            load.release();

            // Diminue le nombre de personne en attente
            // On ne soustrait pas à la fin pour être le plus fidèle possible au modéle
            waitingMutex.acquire();
            nbSkiersWaiting--;
            waitingMutex.release();

            // Incrémente le nombre de personne dans la cabine
            inCabinMutex.acquire();
            nbSkiersInside++;
            inCabinMutex.release();
        }

        // Donne le signale que la cabine démarre
        for(unsigned i = 0; i < nbCanGo; ++i){
            canGo.acquire();
        }
    }
}

void PcoCableCar::unloadSkiers()
{
    qDebug() << "La télécabine décharge les skiers";

    unsigned insideTemp = nbSkiersInside;

    // Laisse les skiers sortir
    for(unsigned i = 0; i < insideTemp; ++i){
        unload.release();

        inCabinMutex.acquire();
        --nbSkiersInside;
        inCabinMutex.release();
    }

    // Attent que tout le monde sort
    for(unsigned i = 0; i < insideTemp ; ++i){
       canGo.acquire();
    }


}
