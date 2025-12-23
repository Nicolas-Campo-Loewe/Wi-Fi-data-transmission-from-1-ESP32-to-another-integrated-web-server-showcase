#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>

const char* ssid = "XXXX";
const char* password = "XXXX";

const char* servidor_ip = "XXX.XXX.XXX.XXX"; 

WebServer server(80);

void handleRequest() {
  String mensaje = obtenerMensajeDeServidor();
  server.send(200, "text/plain", mensaje);
}

String obtenerMensajeDeServidor() {
  HTTPClient http;
  String mensaje;
  
  String url = String("http://") + servidor_ip + "/temperatura";
  http.begin(url);

  int httpCode = http.GET();

  if (httpCode > 0) {
    mensaje = http.getString(); 
    Serial.println("Mensaje del ESP32 servidor recibido con éxito");
    Serial.println("Mensaje enviado al servidor web con éxito");
  } else {
    mensaje = "Error al conectar con el Servidor";
    Serial.println("Error: No se pudo conectar con el ESP32 servidor");
  }

  http.end();
  return mensaje;
}

String paginaWeb = "<html>\
<head>\
<title>Medicion temperatura ESP32 SERVIDOR</title>\
<script>\
function solicitarMensaje() {\
  fetch('/solicitar')\
    .then(response => response.text())\
    .then(data => {\
      const mensajesDiv = document.getElementById('mensajes');\
      const nuevoMensaje = document.createElement('div');\
      nuevoMensaje.textContent = data;\
      mensajesDiv.appendChild(nuevoMensaje);\
    })\
    .catch(error => console.error('Error al solicitar la temperatura:', error));\
}\
</script>\
</head>\
<body>\
<h1>Medicion temperatura ESP32 SERVIDOR</h1>\
<button onclick=\"solicitarMensaje()\">Solicitar Temperatura</button>\
<div id=\"mensajes\"></div>\
</body>\
</html>";

void handleRoot() {
  server.send(200, "text/html", paginaWeb);
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }
  
  Serial.println("Conectado a la red WiFi");
  Serial.print("Dirección IP del ESP32 Cliente: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/solicitar", handleRequest);
  
  server.begin();
  Serial.println("Servidor HTTP iniciado");
}

void loop() {
  server.handleClient(); // Manejar solicitudes HTTP entrantes
}
