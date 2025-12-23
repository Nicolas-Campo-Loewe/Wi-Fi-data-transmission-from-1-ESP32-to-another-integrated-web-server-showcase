#include <WiFi.h>
#include <WebServer.h>
#include <time.h>

const char* ssid = "XXXX";
const char* password = "XXXX";

WebServer server(80);

float obtenerTemperaturaInterna() {
  return temperatureRead();
}

String obtenerHoraUTC() {
  time_t now;
  struct tm timeinfo;
  time(&now);
  gmtime_r(&now, &timeinfo);
  char buffer[32];
  strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
  return String(buffer);
}

void handleTemperatura() {
  float temperatura = obtenerTemperaturaInterna();
  String horaUTC = obtenerHoraUTC();
  String mensaje = "Temperatura interna: " + String(temperatura, 2) + " °C, Hora UTC: " + horaUTC;

  try {
    server.send(200, "text/plain", mensaje);
    Serial.println("Exito");
  } catch (const std::exception& e) {
    Serial.println("Error: No se pudo enviar el mensaje al ESP32 cliente");
    Serial.println(e.what());
  }
}

void handleToggleMedicion() {
  if (server.arg("action") == "start") {
    server.send(200, "text/plain", "Medición iniciada");
  } else if (server.arg("action") == "stop") {
    server.send(200, "text/plain", "Medición detenida");
  } else {
    server.send(400, "text/plain", "Acción no valida");
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }

  Serial.println("Conectado a la red WiFi");

  Serial.print("IP del ESP32 Servidor: ");
  Serial.println(WiFi.localIP());

  configTime(0, 0, "pool.ntp.org", "time.nist.gov");

  server.on("/temperatura", handleTemperatura);
  server.on("/toggleMedicion", handleToggleMedicion);
  
  server.begin();
  Serial.println("Servidor HTTP iniciado");
}

void loop() {
  server.handleClient();
}
