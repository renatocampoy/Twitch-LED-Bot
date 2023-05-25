/*
 * Twitch LED Bot - ESP8266
 * 
 * Desenvolvido por: Eng. Renato Campoy - http://campoy.eng.br
 * Versão: 1.0.0
 * Data: 25/05/2023
 * 
 * Licença: GPLv3
 * 
 * O código é livre para ser usado, modificado e distribuído,
 * desde que a atribuição seja feita ao autor original.
 */

#include <ESP8266WiFi.h>  // Importa a biblioteca para WiFi do ESP8266
#include <DNSServer.h>  // Importa a biblioteca para DNS Server
#include <ESP8266WebServer.h>  // Importa a biblioteca para Web Server do ESP8266
#include <WiFiManager.h>  // Importa a biblioteca para gerenciamento de WiFi
#include <EEPROM.h>  // Importa a biblioteca para armazenamento EEPROM
#include <Adafruit_NeoPixel.h>  // Importa a biblioteca para controle de LEDs NeoPixel

#define EEPROM_SIZE 512  // Define o tamanho da EEPROM
#define LED_PIN D8  // Define o pino onde o LED está conectado
#define LED_COUNT 200  // Define o número de LEDs no strip

// Define uma estrutura para armazenar as credenciais da Twitch
struct {
  char nick[40];
  char token[40];
}
eepromData;

// Define os parâmetros do WiFi Manager para configurar as credenciais da Twitch
WiFiManagerParameter customNick("nick", "Nick", "", 40);
WiFiManagerParameter customToken("token", "Token", "", 40);

// Define as configurações do servidor da Twitch
const char * server = "irc.chat.twitch.tv";
const int port = 6667;

// Define o cliente WiFi e o WiFi Manager
WiFiClient client;
WiFiManager wifiManager;

// Define a configuração do strip de LEDs
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// Função de callback para salvar as configurações na EEPROM quando a configuração do WiFi for concluída
void saveConfigCallback() {
  Serial.println("Deveria salvar para a EEPROM agora");

  strcpy(eepromData.nick, customNick.getValue());  // Copia o nick da Twitch para a estrutura
  strcpy(eepromData.token, customToken.getValue());  // Copia o token da Twitch para a estrutura

  EEPROM.put(0, eepromData);  // Grava a estrutura na EEPROM
  EEPROM.commit();  // Comita as alterações na EEPROM
}

// Configuração inicial do ESP8266
void setup() {
  Serial.begin(115200);  // Inicia a comunicação serial
  EEPROM.begin(EEPROM_SIZE);  // Inicia a EEPROM

  EEPROM.get(0, eepromData);  // Recupera as credenciais da Twitch da EEPROM
  Serial.println("Dados carregados da EEPROM:");
  Serial.println(eepromData.nick);  // Imprime o nick
  Serial.println(eepromData.token);  // Imprime o token

  // Se as credenciais da Twitch não estiverem configuradas, reseta as configurações do WiFi Manager
  if (String(eepromData.nick).length() == 0 || String(eepromData.token).length() == 0) {
    wifiManager.resetSettings();
  }

  // Configura os parâmetros do WiFi Manager com as credenciais da Twitch
  customNick.setValue(eepromData.nick, 40);
  customToken.setValue(eepromData.token, 40);

  // Configura a função de callback para salvar as configurações
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  // Adiciona os parâmetros ao WiFi Manager
  wifiManager.addParameter( & customNick);
  wifiManager.addParameter( & customToken);

  // Configura o timeout do WiFi Manager
  wifiManager.setTimeout(180);

  // Tenta conectar automaticamente ao WiFi
  if (!wifiManager.autoConnect("LEDBOT_AP")) {
    Serial.println("Falha na conexão WiFi... Reiniciando.");  // Se falhar, imprime uma mensagem e reinicia o ESP8266
    delay(3000);
    ESP.restart();
  }

  Serial.println("Conectado!");  // Se conectar, imprime uma mensagem

  // Inicia o strip de LEDs
  strip.begin();
  strip.show();
  
  Serial.println("");
  Serial.println("WiFi connected");

  Serial.println("Starting connection to server...");
  if (client.connect(server, port)) {
    while (client.connected()) {
      Serial.print(client.availableForWrite());
      Serial.print(".");
      client.print("PASS " + String(eepromData.token) + "\r\n");
      client.print("NICK " + String(eepromData.nick) + "\r\n");
      client.print("JOIN #" + String(eepromData.nick) + "\r\n");
      delay(1000);
    }
    Serial.println("connected to server");
  }
}

// Função principal que será executada continuamente
void loop() {
  // Adicionado para contornar problemas de conexão com a twitch
  if (!client.connected()) {
    if (client.connect(server, port)) {
      Serial.print(".");
      client.print("PASS " + String(eepromData.token) + "\r\n");
      client.print("NICK " + String(eepromData.nick) + "\r\n");
      client.print("JOIN #" + String(eepromData.nick) + "\r\n");
    }
  }

  // Comandos via SERIAL
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    if (command.startsWith("reset esp")) {
      Serial.println("Comando recebido: Reiniciando o ESP.");
      ESP.restart();
    } else if (command.startsWith("reset wifi")) {
      Serial.println("Comando recebido: Reiniciando configurações de wifi.");
      wifiManager.resetSettings();
    } else if (command.startsWith("reset twitch")) {
      Serial.println("Comando recebido: Reiniciando configurações da twitch.");
      strcpy(eepromData.token, "");
      strcpy(eepromData.nick, "");
      EEPROM.put(0, eepromData);
      EEPROM.commit();
    } else if (command.startsWith("reset all")) {
      Serial.println("Comando recebido: Reiniciando tudo.");
      strcpy(eepromData.token, "");
      strcpy(eepromData.nick, "");
      EEPROM.put(0, eepromData);
      EEPROM.commit();
      wifiManager.resetSettings();
      ESP.restart();
    }
  }

  while (client.available()) {
    String line = client.readStringUntil('\r');
    int commandIndex = line.indexOf("!led");
    if (commandIndex != -1) {
      String command = line.substring(commandIndex, commandIndex + 4);
      String variable = line.substring(commandIndex + 5);
      String cor = variable;

      variable.toLowerCase();
      cor.toLowerCase();

      Serial.println("Comando recebido: " + command);
      Serial.println("Com a variavel: " + variable);

      // Cores legendas
      if (variable == "cores") {
        String cores = "vermelho, amarelo, ciano, magenta, verde, azul, ciano, magenta, branco, preto, laranja e rosa, ou uma cor em hex exemplo: #03cffc";
        client.print("PRIVMSG #" + String(eepromData.nick) + " :" + "As cores disponíveis são: " + cores + "\r\n");
      }

      // Cores padrões
      if (cor == "vermelho") {
        setColor(255, 0, 0, variable);
      } else if (cor == "amarelo") {
        setColor(255, 255, 0, variable);
      } else if (cor == "ciano") {
        setColor(0, 255, 255, variable);
      } else if (cor == "magenta") {
        setColor(255, 0, 255, variable);
      } else if (cor == "verde") {
        setColor(0, 255, 0, variable);
      } else if (cor == "azul") {
        setColor(0, 0, 255, variable);
      } else if (cor == "ciano") {
        setColor(0, 255, 255, variable);
      } else if (cor == "magenta") {
        setColor(255, 0, 255, variable);
      } else if (cor == "branco") {
        setColor(255, 255, 255, variable);
      } else if (cor == "preto") {
        setColor(0, 0, 0, variable);
      } else if (cor == "laranja") {
        setColor(255, 165, 0, variable);
      } else if (cor == "rosa") {
        setColor(255, 105, 180, variable);
      } else {
        setColor(0, 0, 0, variable);
      }

      // Cores em HEX
      int variableIndex = variable.indexOf("#");
      if (variableIndex != -1) {
        int r, g, b;
        hexToRgb(variable, r, g, b);
        setColor(r, g, b, variable);
      }

      strip.show();

    }
  }
}

// Função para configurar a cor dos LEDs
void setColor(int red, int green, int blue, String variable) {
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(red, green, blue));
  }

  if (variable != "cores") {
    client.print("PRIVMSG #" + String(eepromData.nick) + " :" + "Legal, a cor foi alterada! \r\n");
  }
}

// Função para converter uma cor em formato hexadecimal para RGB
void hexToRgb(String hexColor, int & r, int & g, int & b) {
  char charBuff[7];
  hexColor.toCharArray(charBuff, sizeof(charBuff));
  sscanf(charBuff, "%02x%02x%02x", & r, & g, & b);
}
