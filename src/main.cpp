/*
Projet: Le nom du script
Equipe: P15
Auteurs: Les membres auteurs du script
Description: Breve description du script
Date: Derniere date de modification
*/

/*
Inclure les librairies de functions que vous voulez utiliser
*/
#include <LibRobus.h>

/*
Variables globales et defines
 -> defines...
 -> L'ensemble des fonctions y ont acces
*/

bool bumperArr;
bool sonSifflet;
int vertpin = 48;
int rougepin = 49;
bool vert = false;
bool rouge = false;
int etat = 0; // = 0 arrêt 1 = avance 2 = recule 3 = TourneDroit 4 = TourneGauche
int etatPast = 0;
float vitesse = 0.40;
bool finLabyrinthe = false; // laby fini ou pas
unsigned long tempsDebut = 0; // temps au debut
bool timerDemarre = false;    // check si timer a start

// Tableau pour stocker les mouvements
const int tailleMax = 100;
int mouvements[tailleMax];
int indexMouvements = 0;

/*
Vos propres fonctions sont creees ici
*/

void beep(int count){
  for(int i=0;i<count;i++){
    AX_BuzzerON();
    delay(100);
    AX_BuzzerOFF();
    delay(100);  
  }
  delay(400);
}

//fonctions pour mouvements du prof

void arret(){
  MOTOR_SetSpeed(RIGHT, 0);
  MOTOR_SetSpeed(LEFT, 0);
};

void avance(){
  MOTOR_SetSpeed(RIGHT,vitesse);
  MOTOR_SetSpeed(LEFT, vitesse);
};

void recule(){
  MOTOR_SetSpeed(RIGHT, -0.5*vitesse);
  MOTOR_SetSpeed(LEFT, -vitesse);
};

void tourneDroit(){
  MOTOR_SetSpeed(RIGHT, 0.5*vitesse);
  MOTOR_SetSpeed(LEFT, -0.5*vitesse);
};

void tourneGauche(){
  MOTOR_SetSpeed(RIGHT, -0.5*vitesse);
  MOTOR_SetSpeed(LEFT, 0.5*vitesse);
};

// Fonctions pour enregistrer les mouvements

void enregistrerMouvement(int mouvement){
  if (indexMouvements < tailleMax) {
    mouvements[indexMouvements++] = mouvement;
  }
}

void avancerEtEnregistrer(){
  avance();
  enregistrerMouvement(1);
}

void tournerGaucheEtEnregistrer(){
  tourneGauche();
  enregistrerMouvement(4);
}

void tournerDroitEtEnregistrer(){
  tourneDroit();
  enregistrerMouvement(3);
}

// Fonction pour revenir en arrière
void revenirEnArriere(){
  for (int i = indexMouvements - 1; i >= 0; i--) {
    int dernierMouvement = mouvements[i];
    switch(dernierMouvement){
      case 1:
        recule();
        break;
      case 3:
        tourneGauche();
        break;
      case 4:
        tourneDroit();
        break;
    }
    delay(500);
  }
}

/*
Fonctions d'initialisation (setup)
 -> Se fait appeler au debut du programme
 -> Se fait appeler seulement un fois
 -> Generalement on y initilise les variables globales
*/
void setup(){
  BoardInit();
  
  pinMode(vertpin, INPUT);
  pinMode(rougepin, INPUT);
  delay(100);
  beep(3);
}

/*
Fonctions de boucle infini
 -> Se fait appeler perpetuellement suite au "setup"
*/
void loop() {
  etatPast = etat;
  bumperArr = ROBUS_IsBumper(3);
  
  if (bumperArr){
    if (etat == 0){
      beep(2);
      etat = 1;
      if (!timerDemarre) {
        tempsDebut = millis(); // start timer
        timerDemarre = true;
      }
    } 
    else{
      beep(1);
      etat = 0;
    }
  }
  
  vert = digitalRead(vertpin);
  rouge = digitalRead(rougepin);


//handle le timer
  if (timerDemarre && (millis() - tempsDebut >= 5000)) {
    finLabyrinthe = true;
    timerDemarre = false;
  }

  if (etat > 0 && !finLabyrinthe){
    if (vert && rouge){
      etat = 1;
      avancerEtEnregistrer();
    }
    if (!vert && !rouge){
      etat = 2;
    }
    if (!vert && rouge){
      etat = 3;
      tournerDroitEtEnregistrer();
    }
    if (vert && !rouge){
      etat = 4;
      tournerGaucheEtEnregistrer();
    }
  }

  if (finLabyrinthe){
    arret();
    delay(500);
    beep(3); // Signal de fin de labyrinthe
    revenirEnArriere(); // Faire le chemin inverse
    finLabyrinthe = false;
  }
}

// switch case du prof
//
//   if (etatPast != etat){
//     arret();
//     delay(50);
//   }
//   else{
//     switch (etat)
//     {
//     case 0:
//       arret();
//       break;
//     case 1:
//       avance();
//       break;
//     case 2:
//       recule();
//       break;
//     case 3:
//       tourneDroit();
//       break;
//     case 4:
//       tourneGauche();
//       break;            
//     default:
//       avance();
//       etat = 1;
//       break;
//     }
//   }
//   delay(200);
// }
