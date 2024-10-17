/*
Projet: Défi du labyrinthe
Equipe: P15
Auteurs: Karel Mahiet, Jérémie Perron, Vincent Lebeuf
Description: Algorithme de résolution de labyrinthe
Date: 8 octobre 2024
*/

#include <Arduino.h>
#include <librobus.h>

//Variables pour robotA
int nbBitDroiteA = 1282;
int nbBitGaucheA = 1293;
int nbBitAvantA = 6450;
float vitesseDroiteA = 0.591;
float vitesseGaucheA = 0.615;

//Variables globales
bool intelligenceActive = false;
bool tapeExsistant = true;
bool etatChoisi = false;
bool estAuDepart = true;
bool estEnReverse = false;

int vertPinAvant = 48;
int rougePinAvant = 49;
int vertPinGauche = 38;
int rougePinGauche = 39;
int sonPin = A0;

bool vertAvant = false;
bool rougeAvant = false;
bool vertGauche = false;
bool rougeGauche = false;

int etat = 0; // 0:arrêt, 1:avance, 2:recule, 3:TourneDroit, 4:TourneGauche
int etatPast = 0;

int32_t encodeurGauche = 0;
int32_t encodeurDroite = 0;

int parcoursReverseDroit [15] = {4,1,4,1,1,1,1,1,1,1,1,1,1,1,0};
int parcoursReverseGauche [15] = {3,1,3,1,1,1,1,1,1,1,1,1,1,1,0};
int compteurReverse = 0;
int coteFinal = 0;

//0: Case de contour inconnue (peut-être un mur, il faut utiliser le détecteur de proximité)
//1: Case de contour interdite (tape)
//2: Case de position
//3: Case de postion de fin de parcours droit
//4: Case de postion de fin de parcours gauche
int parcours [23][7] = 
    {{1,1,1,1,1,1,1},
     {1,4,1,2,1,3,1},
     {1,0,1,1,1,0,1},
     {1,2,1,2,1,2,1},
     {1,0,1,1,1,0,1},
     {1,2,0,2,0,2,1},
     {1,0,1,1,1,0,1},
     {1,2,1,2,1,2,1},
     {1,0,1,1,1,0,1},
     {1,2,0,2,0,2,1},
     {1,0,1,0,1,0,1},
     {1,2,1,2,1,2,1},
     {1,0,1,0,1,0,1},
     {1,2,0,2,0,2,1},
     {1,0,1,1,1,0,1},
     {1,2,1,2,1,2,1},
     {1,0,1,1,1,0,1},
     {1,2,0,2,0,2,1},
     {1,0,1,1,1,0,1},
     {1,2,1,2,1,2,1},
     {1,0,1,1,1,0,1},
     {1,2,0,2,0,2,1},
     {1,1,1,1,1,1,1}};

int ligneCourante = 21;
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

void ChoisirEtatAller()
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

void arret(){
  MOTOR_SetSpeed(RIGHT, 0);
  MOTOR_SetSpeed(LEFT, 0);
};

void avance(){
  MOTOR_SetSpeed(RIGHT, 0.5*vitesseDroiteA);
  MOTOR_SetSpeed(LEFT, 0.5*vitesseGaucheA);

  encodeurDroite = ENCODER_Read(1);

  if(encodeurDroite > 1000)
  {
      MOTOR_SetSpeed(RIGHT, vitesseDroiteA);
      MOTOR_SetSpeed(LEFT, vitesseGaucheA);
  }

  if(encodeurDroite > 5000)
  {
      MOTOR_SetSpeed(RIGHT, 0.5*vitesseDroiteA);
      MOTOR_SetSpeed(LEFT, 0.5*vitesseGaucheA);
  }

  if(encodeurDroite > nbBitAvantA) //Lorsque les 50cm sont complétés
  {
    ENCODER_Reset(0);
    ENCODER_Reset(1);
    AjusterPositionActuelle();
    arret();
    delay(280);

     if(!estEnReverse)
     {
         etatChoisi = false;
     }
     else
     {
       compteurReverse++;
     }
  }
};

void tourneDroit(){
  MOTOR_SetSpeed(RIGHT, -0.5/*-0.5*vitesseDroiteA*/);
  MOTOR_SetSpeed(LEFT, 0.5/*0.5*vitesseGaucheA*/);

  encodeurGauche = ENCODER_Read(0);

  if(encodeurGauche > nbBitDroiteA) //Lorsque le 90 degré est complété
  {
    ENCODER_Reset(0);
    ENCODER_Reset(1);
    AjusterDirectionDroite();
    arret();
    delay(280);

    if(!estEnReverse)
    {
        etatChoisi = false;
    }
    else
     {
       compteurReverse++;
     }
  }
};

void tourneGauche(){
  MOTOR_SetSpeed(RIGHT, 0.5/*0.5*vitesseDroiteA*/);
  MOTOR_SetSpeed(LEFT, -0.5/*-0.5*vitesseGaucheA*/);

  encodeurDroite = ENCODER_Read(1);

  if(encodeurDroite > nbBitGaucheA) //Lorsque le 90 degré est complété
  {
    ENCODER_Reset(0);
    ENCODER_Reset(1);
    AjusterDirectionGauche();
    arret();
    delay(280);

    if(!estEnReverse)
    {
        etat = 1;
    }
    else
     {
       compteurReverse++;
     }
  }
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

void loop()
{
  etatPast = etat;
  if (estAuDepart && !intelligenceActive && (analogRead(sonPin) >= 550 || ROBUS_IsBumper(3))){
      beep(2);
      intelligenceActive = true;
      estAuDepart = false;
  }
  
  vertAvant = digitalRead(vertPinAvant);
  rougeAvant = digitalRead(rougePinAvant);
  vertGauche = digitalRead(vertPinGauche);
  rougeGauche = digitalRead(rougePinGauche);
  if (intelligenceActive && !etatChoisi){
    ChoisirEtatAller();
  }

  if (etatPast != etat){ //fait une pause minimale entre les changements d'état
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

  //Lorsque le robot arrive dans une des 3 cases de fin, on lance la rotation de 180 degrés
  if((parcours[ligneCourante][colonneCourante] == 3 || parcours[ligneCourante][colonneCourante] == 4) && !estEnReverse)
  {
      estEnReverse = true;
      intelligenceActive = false;
      etatChoisi = true;
      if(parcours[ligneCourante][colonneCourante] == 3)
      {
         coteFinal = 3;
      }
      else
      {
         coteFinal = 4;
      }
  }

  //À partir du moment où le parcours d'allée est complété, on défini l'état avec la matrice reverse
  if(estEnReverse)
  {
    if(coteFinal == 3 && compteurReverse < 15)
    {
        etat = parcoursReverseDroit[compteurReverse];
    }
    if(coteFinal == 4 && compteurReverse < 15)
    {
        etat = parcoursReverseGauche[compteurReverse];
    }
  }

  delay(200);
}