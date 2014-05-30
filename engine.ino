#include <RelaisManager.h>
#include <WebServer.h>
#include <SPI.h>
#include <Ethernet.h>


IPAddress ipLocal(192, 168, 1, 101);
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0F, 0x50, 0x4E };

WebServer webserver("", 80);

RelaisManager relaisServo(2, 8);

RelaisManager relaisVitesseOne(3, 9);
RelaisManager relaisVitesseTwo(4, 10);
RelaisManager relaisVitesseThree(5, 11);



/**********************/
/** HTTP POST SYSTEM **/
/**********************/

// Variable pour stocker les noms des donnees
char postNameDatas[16][16];
// Variable pour stocker les valeurs des donnees
char postValueDatas[16][16];
// Variable de la taille des donnees
int postDatasSize = 0;

/**
 *
 * Recupere les donnees de la requete courrante POST
 * et les stock dans un tableau et retourne celle demandee
 * @param WebServer server Instance la classe Webserver de la requete courante
 * @param String wanted Le nom de la donnee que l'on veut recuperer
 *
 * @return String
 *
 */
String getPostDatas(WebServer &server, String wanted)
{
  bool repeat = true;
  char name[16], value[16];

  // Si aucune donnee n'est stockee dans le tableau on parcour celle de la requete
  if (postDatasSize <= 0) {
    // Tant que tout n'est pas lu
    while(repeat) {
      repeat = server.readPOSTparam(name, 16, value, 16);
      String strName(name);

      if (strName != "") {
        // On stock le nom de la donnee
        strcpy(postNameDatas[postDatasSize], name);
        // On stock la valeur de la donnee
        strcpy(postValueDatas[postDatasSize], value);
        
        postDatasSize++;
      }
    }
  }

  // On parcoure le tableau de stockage des donnees
  for(int i = 0; i < postDatasSize; i++) {
    char * dataName = postNameDatas[i];
    char * dataValue = postValueDatas[i];

    String strDataName(dataName);
    String strDataValue(dataValue);

    // Si la donnee courante correspond a la donnee demande
    // on retourne sa valeur a l'utilisateur
    if (strDataName == wanted) {
      return dataValue;
    }
  }
  
  return "";
  
}

/**
 *
 * Nettoie le tableau de donnees POST
 * A executer en chaque fin de requete HTTP
 *
 */
void cleanPostDatas()
{
  char empty[1];

  for (int i = 0; i < postDatasSize; i++) {
    strcpy(postNameDatas[i], empty);
    strcpy(postValueDatas[i], empty);
  }

  postDatasSize = 0;
}



void SpeedCmd (WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete) { 

    String rawVitesse = getPostDatas(server, "vitesse");
    int Vitesse = rawVitesse.toInt();
    cleanPostDatas();

    Serial.println(Vitesse);

    switch (Vitesse) {
        case 1:
            relaisVitesseThree.off();
            relaisVitesseTwo.off();
            relaisVitesseOne.on();
            break;
        case 2:
            relaisVitesseOne.off();
            relaisVitesseThree.off();
            relaisVitesseTwo.on();
            break;
        case 3:
            relaisVitesseOne.off();
            relaisVitesseTwo.off();
            relaisVitesseThree.on();
            break;
        case 4:
            relaisServo.on();
            break;
        case 5:
            relaisServo.off();
            break;
        case 0:
            relaisVitesseOne.off();
            relaisVitesseTwo.off();
            relaisVitesseThree.off();

            relaisServo.off();
            break;
    }

    server.httpSuccess();
    server.print("OK");
}

void setup()
{
    Serial.begin(115200);

    Ethernet.begin(mac, ipLocal);
    webserver.addCommand("speed", &SpeedCmd);
}

void loop()
{
    webserver.processConnection();
}