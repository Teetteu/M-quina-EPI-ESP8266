#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include <Servo.h>

#define M1_IN1 5    // D1
#define M1_IN2 4    // D2
#define M1_ENA 13   // D7

#define SERVO2_PIN 15   // D8
#define SERVO3_PIN 12   // D6

#define LED_PIN 2   // D4

ESP8266WebServer server(80);
Servo servo2;
Servo servo3;

const char* ssid = "MáquinaEPI";
const char* password = "12345678";

void acionarMotor1() {
  digitalWrite(M1_IN1, HIGH);
  digitalWrite(M1_IN2, LOW);
  analogWrite(M1_ENA, 1023);
  delay(2500); // 2.5 segundos
  digitalWrite(M1_IN1, LOW);
  digitalWrite(M1_IN2, LOW);
  analogWrite(M1_ENA, 0);
}

void acionarMotor2() {
  servo2.write(180); // Gira
  delay(2000);       // 2.0 segundos
  servo2.write(90);  // Para
}

void acionarMotor3() {
  servo3.write(180);
  delay(2300);       // 2.3 segundos
  servo3.write(90);
}

void acionarLed() {
  digitalWrite(LED_PIN, LOW);   // Liga (ativo em LOW)
  delay(2000);                  // Aguarda 2 segundos
  digitalWrite(LED_PIN, HIGH);  // Desliga
}

void handleFileRequest() {
  String path = server.uri();
  if (path == "/") path = "/index.html";

  if (LittleFS.exists(path)) {
    File file = LittleFS.open(path, "r");
    String contentType = "text/plain";
    if (path.endsWith(".html")) contentType = "text/html";
    else if (path.endsWith(".css")) contentType = "text/css";
    else if (path.endsWith(".js")) contentType = "application/javascript";
    else if (path.endsWith(".png")) contentType = "image/png";
    else if (path.endsWith(".jpg")) contentType = "image/jpeg";
    server.streamFile(file, contentType);
    file.close();
  } else {
    server.send(404, "text/plain", "Arquivo não encontrado");
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(M1_IN1, OUTPUT); pinMode(M1_IN2, OUTPUT); pinMode(M1_ENA, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH); // Desligado (ativo em LOW)

  servo2.attach(SERVO2_PIN);
  servo3.attach(SERVO3_PIN);
  servo2.write(90);
  servo3.write(90);

  WiFi.softAP(ssid, password);
  Serial.println("Ponto de acesso iniciado.");
  Serial.print("IP: "); Serial.println(WiFi.softAPIP());

  if (!LittleFS.begin()) {
    Serial.println("Erro ao montar LittleFS");
    return;
  }

  // Rotas
  server.on("/led/on", []() {
    acionarLed();
    server.send(200, "text/plain", "LED acionado por 2 segundos");
  });

  server.on("/motor/1/on", []() {
    acionarMotor1();
    server.send(200, "text/plain", "Motor 1 acionado");
  });

  server.on("/motor/2/on", []() {
    acionarMotor2();
    server.send(200, "text/plain", "Motor 2 (servo 360) acionado");
  });

  server.on("/motor/3/on", []() {
    acionarMotor3();
    server.send(200, "text/plain", "Motor 3 (servo 360) acionado");
  });

  server.onNotFound(handleFileRequest);
  server.begin();
  Serial.println("Servidor iniciado.");
}

void loop() {
  server.handleClient();
}
