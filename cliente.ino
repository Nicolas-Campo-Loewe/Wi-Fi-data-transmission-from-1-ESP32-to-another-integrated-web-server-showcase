#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>

// Configuración de red WiFi
const char* ssid = "Cudy-A8EC";
const char* password = "15092717";

// IP del ESP32 Servidor
const char* servidor_ip = "192.168.10.102"; // Ajusta la IP según tu configuración

// Crear servidor web en el puerto 80
WebServer server(80);

// Función para manejar solicitud desde la web
void handleRequest() {
  String mensaje = obtenerMensajeDeServidor();
  server.send(200, "text/plain", mensaje);
}

// Función para obtener mensaje del ESP32 Servidor
String obtenerMensajeDeServidor() {
  HTTPClient http;
  String mensaje;

  // Construir la URL de la solicitud al servidor
  String url = String("http://") + servidor_ip + "/temperatura";
  http.begin(url);

  int httpCode = http.GET(); // Hacer solicitud GET

  if (httpCode > 0) {
    mensaje = http.getString(); // Obtener respuesta si el código es positivo
    Serial.println("Mensaje del ESP32 servidor recibido con éxito");
    Serial.println("Mensaje enviado al servidor web con éxito");
  } else {
    mensaje = "Error al conectar con el Servidor";
    Serial.println("Error: No se pudo conectar con el ESP32 servidor");
  }

  http.end();
  return mensaje;
}

// Página web con JavaScript para solicitudes dinámicas
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
  
  // Esperar hasta conectar a WiFi
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }

  // Una vez conectado, imprimir la IP asignada
  Serial.println("Conectado a la red WiFi");
  Serial.print("Dirección IP del ESP32 Cliente: ");
  Serial.println(WiFi.localIP());

  // Configurar rutas del servidor web
  server.on("/", handleRoot);
  server.on("/solicitar", handleRequest);
  
  server.begin();
  Serial.println("Servidor HTTP iniciado");
}

void loop() {
  server.handleClient(); // Manejar solicitudes HTTP entrantes
}
