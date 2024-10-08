/*
Projet: Défi du labyrinthe
Equipe: P15
Auteurs: Jérémie Perron
Description: Algorithme de résolution de labyrinthe
Date: 26 octobre 2024
*/

#include <Arduino.h>
#include <librobus.h>

bool intelligenceActive = false;
bool tapeExsistant = true;
bool etatChoisi = false;
bool estAuDepart = true;

int vertPinAvant = 48;
int rougePinAvant = 49;
int vertPinGauche = 38;
int rougePinGauche = 39;
int sonPin = A0;

bool vertAvant = false;
bool rougeAvant = false;
bool vertGauche = false;
bool rougeGauche = false;

// Tableau pour stocker les mouvements
const int tailleMax = 100;
int mouvements[tailleMax];
int indexMouvements = 0;

int etat = 0; // 0:arrêt, 1:avance, 2:recule, 3:TourneDroit, 4:TourneGauche
int etatPast = 0;

float vitesseRight = 0.40;
float vitesseLeft = 0.40;

int32_t encodeurGauche = 0;
int32_t encodeurDroite = 0;


//0: Case de contour inconnue (peut-être un mur, il faut utiliser le détecteur de proximité)
//1: Case de contour interdite (tape)
//2: Case de position
//3: Case de postion de fin de parcours
int parcours [21][7] = 
    {{1,1,1,1,1,1,1},
     {1,3,1,3,1,3,1},
     {1,0,1,0,1,0,1},
     {1,2,0,2,0,2,1},
     {1,0,1,0,1,0,1},
     {1,2,1,2,1,2,1},
     {1,0,1,0,1,0,1},
     {1,2,0,2,0,2,1},
     {1,0,1,0,1,0,1},
     {1,2,1,2,1,2,1},
     {1,0,1,0,1,0,1},
     {1,2,0,2,0,2,1},
     {1,0,1,0,1,0,1},
     {1,2,1,2,1,2,1},
     {1,0,1,0,1,0,1},
     {1,2,0,2,0,2,1},
     {1,0,1,0,1,0,1},
     {1,2,1,2,1,2,1},
     {1,0,1,0,1,0,1},
     {1,2,0,2,0,2,1},
     {1,1,1,1,1,1,1}};

int ligneCourante = 19;
int colonneCourante = 3;
int direction = 1; //1: avant, 2: arrière, 3:droit, 4:gauche

void DetecterTapeAvant()
{
  switch (direction)
    {
    case 1: //regarde vers l'avant
      if(parcours[ligneCourante-1][colonneCourante] == 0)
      {
          tapeExsistant = false;
      }    
      else
      {
          tapeExsistant = true;
      }
      break;
    case 2: //regarde vers l'arrière
      if(parcours[ligneCourante+1][colonneCourante] == 0)
      {
          tapeExsistant = false;
      }    
      else
      {
          tapeExsistant = true;
      }
      break;
    case 3: //regarde vers la droite
      if(parcours[ligneCourante][colonneCourante+1] == 0)
      {
          tapeExsistant = false;
      }    
      else
      {
          tapeExsistant = true;
      }
      break;
    case 4: //regarde vers la gauche
      if(parcours[ligneCourante][colonneCourante-1] == 0)
      {
          tapeExsistant = false;
      }    
      else
      {
          tapeExsistant = true;
      }
      break;
    }
}

void DetecterTapeGauche()
{
  switch (direction)
    {
    case 1: //regarde vers l'avant
      if(parcours[ligneCourante][colonneCourante-1] == 0)
      {
          tapeExsistant = false;
      }    
      else
      {
          tapeExsistant = true;
      }
      break;
    case 2: //regarde vers l'arrière
      if(parcours[ligneCourante][colonneCourante+1] == 0)
      {
          tapeExsistant = false;
      }    
      else
      {
          tapeExsistant = true;
      }
      break;
    case 3: //regarde vers la droite
      if(parcours[ligneCourante-1][colonneCourante] == 0)
      {
          tapeExsistant = false;
      }    
      else
      {
          tapeExsistant = true;
      }
      break;
    case 4: //regarde vers la gauche
      if(parcours[ligneCourante+1][colonneCourante] == 0)
      {
          tapeExsistant = false;
      }    
      else
      {
          tapeExsistant = true;
      }
      break;
    }
}

void AjusterPositionActuelle()
{
  switch (direction)
    {
    case 1: //regarde vers l'avant
      ligneCourante = ligneCourante - 2;
      colonneCourante = colonneCourante;
      break;
    case 2: //regarde vers l'arrière
      ligneCourante = ligneCourante + 2;
      colonneCourante = colonneCourante;
      break;
    case 3: //regarde vers la droite
      ligneCourante = ligneCourante;
      colonneCourante = colonneCourante + 2;
      break;
    case 4: //regarde vers la gauche
      ligneCourante = ligneCourante;
      colonneCourante = colonneCourante - 2;
      break;
    }
}

void AjusterDirectionGauche()
{
    switch (direction)
    {
    case 1: //regarde vers l'avant
      direction = 4;
      break;
    case 2: //regarde vers l'arrière
      direction = 3;
      break;
    case 3: //regarde vers la droite
      direction = 1;
      break;
    case 4: //regarde vers la gauche
      direction = 2;
      break;
    }
}

void AjusterDirectionDroite()
{
    switch (direction)
    {
    case 1: //regarde vers l'avant
      direction = 3;
      break;
    case 2: //regarde vers l'arrière
      direction = 4;
      break;
    case 3: //regarde vers la droite
      direction = 2;
      break;
    case 4: //regarde vers la gauche
      direction = 1;
      break;
    }
}

void ChoisirEtat()
{
    DetecterTapeGauche();
    if (vertGauche && rougeGauche && !tapeExsistant){ // si aucun obstacle à gauche => tourne à gauche et avance
      etat = 4;
    }
    else
    {
      DetecterTapeAvant();
      if (vertAvant && rougeAvant && !tapeExsistant){ // si aucun obstacle en avant => avance
        etat = 1;
      }
      else // tourne à droite et la boucle de décision recommence
      {
        etat = 3;
      }
    }
    etatChoisi = true;
}

void beep(int count){
  for(int i=0;i<count;i++){
    AX_BuzzerON();
    delay(100);
    AX_BuzzerOFF();
    delay(100);  
  }
  delay(400);
}

// Fonction pour enregistrer les mouvements
void enregistrerMouvement(int mouvement) {
  if (indexMouvements < tailleMax) {
    mouvements[indexMouvements++] = mouvement;
  }
}

void arret(){
  MOTOR_SetSpeed(RIGHT, 0);
  MOTOR_SetSpeed(LEFT, 0);

  enregistrerMouvement(0);
};

void avance(){
  MOTOR_SetSpeed(RIGHT,vitesseRight);
  MOTOR_SetSpeed(LEFT, vitesseLeft);

  encodeurDroite = ENCODER_Read(1);

  if(encodeurDroite > 5200) //À faire lorsque les 50cm sont complétés
  {
    encodeurDroite = ENCODER_ReadReset(1);
    AjusterPositionActuelle();
    etatChoisi = false;
  }

  enregistrerMouvement(1);

  //À faire lorsque les 50cm sont complétés
  //AjusterPositionActuelle();
  //etatChoisi = false;
};

// Fonction pour reculer
void recule() {
  MOTOR_SetSpeed(RIGHT, -vitesseRight);
  MOTOR_SetSpeed(LEFT, -vitesseLeft);

  encodeurDroite = ENCODER_Read(1);

  if(encodeurDroite > 5200) //À faire lorsque les 50cm sont complétés
  {
    encodeurDroite = ENCODER_ReadReset(1);
    AjusterPositionActuelle();
    etatChoisi = false;
  }

  //À faire lorsque les 50cm sont complétés
  //AjusterPositionActuelle();
  //etatChoisi = false;
}


void tourneDroit(){
  MOTOR_SetSpeed(RIGHT, -0.5*vitesseRight);
  MOTOR_SetSpeed(LEFT, 0.5*vitesseLeft);

  encodeurDroite = ENCODER_Read(1);

  if(encodeurDroite > 900) //À faire lorsque le 90 degré est complété
  {
    encodeurDroite = ENCODER_ReadReset(1);
    AjusterDirectionDroite();
    etatChoisi = false;
  }

  enregistrerMouvement(3);

  //À faire lorsque le 90 degré est complété
  //AjusterDirectionDroite();
  //etatChoisi = false;
};

void tourneGauche(){
  MOTOR_SetSpeed(RIGHT, 0.5*vitesseRight);
  MOTOR_SetSpeed(LEFT, -0.5*vitesseLeft);

  encodeurDroite = ENCODER_Read(1);

  if(encodeurDroite > 900) //À faire lorsque le 90 degré est complété
  {
    encodeurDroite = ENCODER_ReadReset(1);
    AjusterDirectionGauche();
    delay(200);
    etat = 1;
  }

  enregistrerMouvement(4);

  //À faire lorsque le 90 degré est complété
  //AjusterDirectionGauche();
  //etat = 1;
};

void setup()
{
  BoardInit();
  pinMode(vertPinAvant, INPUT);
  pinMode(rougePinAvant, INPUT);
  pinMode(vertPinGauche, INPUT);
  pinMode(rougePinGauche, INPUT);
  pinMode(sonPin, INPUT);
  delay(100);
  beep(3);
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

void loop()
{
  etatPast = etat;
  if (estAuDepart && !intelligenceActive && analogRead(sonPin) >= 550){
      beep(2);
      intelligenceActive = true;
      estAuDepart = false;
  }
  
  vertAvant = digitalRead(vertPinAvant);
  rougeAvant = digitalRead(rougePinAvant);
  vertGauche = digitalRead(vertPinGauche);
  rougeGauche = digitalRead(rougePinGauche);
  if (intelligenceActive && !etatChoisi){
    ChoisirEtat();
  }

  if (etatPast != etat){ //fait une pause entre les changements d'état
    arret();
    delay(50);
  }
  else{
    switch (etat)
    {
    case 0:
      arret();
      break;
    case 1:
      avance();
      break;
    case 3:
      tourneDroit();
      break;
    case 4:
      tourneGauche();
      break;            
    default:
      avance();
      etat = 1;
    break;
    }
  }

  //Arrêt du robot lorsqu'il arrive dans une des 3 cases de fin et lancement des déplacements inverses
  if(parcours[ligneCourante][colonneCourante] == 3)
  {
    intelligenceActive = false;
    etatChoisi = true;
    etat = 0;
    //FONCTION KAREL POUR LE RETOUR
    revenirEnArriere();
  }
  delay(200);
}