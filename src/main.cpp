/*
Projet: Robot A15
Equipe: P15
Auteurs: Karel Mahiet, Dylan Gagnon, Marek Doucet
Description: Fait déplacer le robot dans le labyrinthe et évite les zones avec "1" dans la matrice.
Date: 26 septembre 2024
*/

#include <LibRobus.h>

// Définir les pins pour les deux capteurs
int vertpinAvant = 48;   // Capteur avant vert
int rougepinAvant = 49;  // Capteur avant rouge
int vertpinGauche = 38;  // Capteur gauche vert
int rougepinGauche = 39; // Capteur gauche rouge

bool vertAvant = false;
bool rougeAvant = false;
bool vertGauche = false;
bool rougeGauche = false;

int etat = 0; // 0 = arrêt, 1 = avance, 2 = recule, 3 = tourne droite, 4 = tourne gauche
float vitesse = 0.40;
bool finLabyrinthe = false;

// Variables de position du robot dans la matrice
int posX = 1; // Colonne initiale (à ajuster selon votre position de départ)
int posY = 1; // Ligne initiale (à ajuster selon votre position de départ)

// Tableau pour stocker les mouvements
const int tailleMax = 100;
int mouvements[tailleMax];
int indexMouvements = 0;

// matrice pour tape
const int lignes = 21;
const int colonnes = 7;
int tapeMap[lignes][colonnes] = {
  {1, 1, 1, 1, 1, 1, 1},
  {1, 0, 2, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 1},
  {1, 1, 1, 1, 1, 1, 1},
  {1, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 1},
  {1, 1, 1, 1, 1, 1, 1}
};

// Fonction pour faire bip
void beep(int count) {
  for (int i = 0; i < count; i++) {
    AX_BuzzerON();
    delay(100);
    AX_BuzzerOFF();
    delay(100);
  }
  delay(400);
}

// Fonction pour avancer
void avance() {
  MOTOR_SetSpeed(RIGHT, vitesse);
  MOTOR_SetSpeed(LEFT, vitesse);
}

// Fonction pour reculer
void recule() {
  MOTOR_SetSpeed(RIGHT, -0.5 * vitesse);
  MOTOR_SetSpeed(LEFT, -vitesse);
}

// Fonction pour tourner à droite
void tourneDroit() {
  MOTOR_SetSpeed(RIGHT, 0.5 * vitesse);
  MOTOR_SetSpeed(LEFT, -0.5 * vitesse);
}

// Fonction pour tourner à gauche
void tourneGauche() {
  MOTOR_SetSpeed(RIGHT, -0.5 * vitesse);
  MOTOR_SetSpeed(LEFT, 0.5 * vitesse);
}

// Fonction pour arrêter
void arret() {
  MOTOR_SetSpeed(RIGHT, 0);
  MOTOR_SetSpeed(LEFT, 0);
}

// Fonction pour enregistrer les mouvements
void enregistrerMouvement(int mouvement) {
  if (indexMouvements < tailleMax) {
    mouvements[indexMouvements++] = mouvement;
  }
}

// Fonction pour tourner à droite et enregistrer
void tournerDroitEtEnregistrer() {
  tourneDroit();
  enregistrerMouvement(3);
}

// Fonction pour tourner à gauche et enregistrer
void tournerGaucheEtEnregistrer() {
  tourneGauche();
  enregistrerMouvement(4);
}

// Fonction d'initialisation (setup)
void setup() {
  BoardInit();
  pinMode(vertpinAvant, INPUT);
  pinMode(rougepinAvant, INPUT);
  pinMode(vertpinGauche, INPUT);
  pinMode(rougepinGauche, INPUT);
  delay(100);
  beep(3);
}

// Fonction pour vérifier si la prochaine position contient du tape
bool estSurTape(int x, int y) {
  if (x >= 0 && x < colonnes && y >= 0 && y < lignes) {
    return tapeMap[y][x] == 1; // Si la case contient un 1 (tape)
  }
  return false;
}

// Fonction pour avancer et enregistrer
void avancerEtEnregistrer() {
  // Vérifier si la prochaine position contient du tape
  int prochainePosX = posX; // Ajuster en fonction de la direction
  int prochainePosY = posY + 1; // Ex: avancer

  if (!estSurTape(prochainePosX, prochainePosY)) {
    avance();
    posY = prochainePosY; // Mettre à jour la position du robot
    enregistrerMouvement(1);
  } else {
    // Si tape détecté, tourner ou éviter l'obstacle
    beep(2); // Faire un signal sonore
    tournerGaucheEtEnregistrer(); // Exemple d'évitement
  }
}

// Fonction pour revenir en arrière
void revenirEnArriere() {
  for (int i = indexMouvements - 1; i >= 0; i--) {
    int dernierMouvement = mouvements[i];
    switch (dernierMouvement) {
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

// Boucle principale (loop)
void loop() {
  // Lire les états des capteurs
  vertAvant = digitalRead(vertpinAvant);
  rougeAvant = digitalRead(rougepinAvant);
  vertGauche = digitalRead(vertpinGauche);
  rougeGauche = digitalRead(rougepinGauche);

  // Déterminer l'état basé sur les capteurs
  if (analogRead(A0) >= 550) {
    etat = 1; // Avancer si le son est détecté
  }

  if (etat > 0 && !finLabyrinthe) {
    // Si capteur avant détecte obstacle
    if (vertAvant && rougeAvant) {
      etat = 1;  // Avancer
    } 
    // Si aucun capteur n'est actif
    else if (!vertAvant && !rougeAvant) {
      etat = 2;  // Reculer
    }
    // Si capteur avant rouge détecte un obstacle
    else if (!vertAvant && rougeAvant) {
      etat = 3;  // Tourner à droite
    }
    // Si capteur gauche vert détecte un obstacle
    else if (vertGauche && !rougeGauche) {
      etat = 4;  // Tourner à gauche
    }

    // Exécuter l'action selon l'état
    switch (etat) {
      case 1:
        avancerEtEnregistrer();
        break;
      case 2:
        recule();
        break;
      case 3:
        tournerDroitEtEnregistrer();
        break;
      case 4:
        tournerGaucheEtEnregistrer();
        break;
      default:
        arret();
        break;
    }
  }

  // Si le labyrinthe est terminé
  if (finLabyrinthe) {
    arret();
    delay(500);
    beep(3);
    revenirEnArriere();
    finLabyrinthe = false;
  }
}
