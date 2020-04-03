//    ___  _________    ___  ___  ___  ___  //
//   / _ \/ ___/ __ \  |_  |/ _ \|_  |/ _ \ //
//  / ___/ /__/ /_/ / / __// // / __// // / //
// /_/   \___/\____/ /____/\___/____/\___/  //
//                                          //
// Auteurs : Mattei Simon, Kot Chau Ying

#include "cablecarbehavior.h"

void CableCarBehavior::run()
{
    // A vous d'ajouter le comportement du télécabine
    while(cableCar->isInService()){
        cableCar->loadSkiers();
        cableCar->goUp();
        cableCar->unloadSkiers();
        cableCar->goDown();
    }
}
