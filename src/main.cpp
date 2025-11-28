#include <Arduino.h>
#include <WiFi.h>
#include <DHT.h>
#include <DHT_U.h>
#include <PubSubClient.h>
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>


#define DHTPIN    4 
#define waterpump 18
#define soilPin   5

#define DHTTYPE DHT11   
DHT dht(DHTPIN, DHTTYPE);

#if defined(ESP32)
#include <WiFiMulti.h>
WiFiMulti wifiMulti;
#define DEVICE "ESP32"
#elif defined(ESP8266)
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti wifiMulti;
#define DEVICE "ESP8266"
#endif



// WiFi AP SSID
#define WIFI_SSID         "Gustavo"
#define WIFI_PASSWORD     "mqtteste"

#define INFLUXDB_URL      "https://us-east-1-1.aws.cloud2.influxdata.com"
#define INFLUXDB_TOKEN    "fzN82wK6GrGkO1qb3zLh7ygG9s4VxdZhdp4cq2CFfl2QlXlT2bChG3932HrISQzcdDlz9xUlYzvsfsxuLieMyA=="
#define INFLUXDB_ORG      "ac342d73b369637c"
#define INFLUXDB_BUCKET   "Estufa IOT"
//#define DEVICE            "estufa_001"
#define INFLUXDB_SEND_TIME    (10000u) //10s
#define DHT11_REFRESH_TIME    (5000u) //5s
#define SENSOR_BUFFER_SIZE    (INFLUXDB_SEND_TIME/DHT11_REFRESH_TIME)
// Time zone info
#define TZ_INFO           "UTC-3"

// Declare InfluxDB client instance with preconfigured InfluxCloud certificate
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

// Declare Data point
Point sensor("wifi_status");

static uint32_t dht_refresh_timestamp = 0u;
static uint32_t influxdb_send_timestamp = 0u;
static void WiFi_Setup( void );
static void DHT11_TaskInit( void );
static void DHT11_TaskMng( void );
static void InfluxDB_TaskInit( void );
static void InfluxDB_TaskMng( void );
static float Get_HumidityValue( void );
static float Get_TemperatureValue( void );
//variaveis
float dht_temperature = 0;
float dht_humidity = 0;
static uint8_t temp_buffer[SENSOR_BUFFER_SIZE] = { 0 };
static uint8_t humidity_buffer[SENSOR_BUFFER_SIZE] = { 0 };
static uint16_t sensor_buffer_idx = 0;

void setup() {
  Serial.begin(115200);
  dht.begin();
  WiFi_Setup();
  InfluxDB_TaskInit();
  DHT11_TaskInit();
 
 sensor.addTag("SSID", WiFi.SSID());
  
}
void loop() {
  DHT11_TaskMng();
  InfluxDB_TaskMng();
}
static void WiFi_Setup(void){
   // Setup wifi
  Serial.println("Connecting to wifi");
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);

  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println("Connected to WiFi");
  Serial.println();
}
static void DHT11_TaskInit( void ){
  dht.begin();
  // delay(2000);
  dht_refresh_timestamp = millis();
}
static void DHT11_TaskMng( void ){
   uint32_t now = millis();
  float temperature, humidity;
  if( now - dht_refresh_timestamp >= DHT11_REFRESH_TIME ){
    dht_refresh_timestamp = now;
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    humidity = dht.readHumidity();
    // Read temperature as Celsius (the default)
    temperature = dht.readTemperature();
    // Check if any reads failed and exit early (to try again).
    if (isnan(humidity) || isnan(temperature) ) 
    {
      Serial.println(F("Failed to read from DHT sensor!"));
    }
    else
    {
      Serial.print(F("Humidity: "));
      Serial.print(humidity);
      Serial.print(F("%  Temperature: "));
      Serial.print(temperature);
      Serial.println(F("°C "));
      // store this in the global variables
      dht_humidity = humidity;
      dht_temperature = temperature;
      temp_buffer[sensor_buffer_idx] = dht_temperature;
      humidity_buffer[sensor_buffer_idx] = dht_humidity;
      sensor_buffer_idx++;
      if( sensor_buffer_idx >= SENSOR_BUFFER_SIZE )
      {
        sensor_buffer_idx = 0;
      }
    }
  }
}
static void InfluxDB_TaskInit(void){
  sensor.addTag("device", DEVICE);
  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");
  // teste de conexão com o influxdb
   if (client.validateConnection()) {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(client.getServerUrl());
  } else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
  }
}
static void InfluxDB_TaskMng(void){
  uint32_t now = millis();
  if( now - influxdb_send_timestamp >= INFLUXDB_SEND_TIME )
  {
    influxdb_send_timestamp = now;
    sensor.clearFields();
    // Report RSSI of currently connected network
    sensor.addField( "rssi", WiFi.RSSI() );
    // add temperature and humidity values also
    sensor.addField( "temperature", Get_TemperatureValue() );
    sensor.addField( "humidity", Get_HumidityValue() );
    // Print what are we exactly writing
    Serial.print("Writing: ");
    Serial.println(client.pointToLineProtocol(sensor));
    // If no Wifi signal, try to reconnect it
    if (wifiMulti.run() != WL_CONNECTED)
    {
      Serial.println("Wifi connection lost");
    }
    // Write point
    if (!client.writePoint(sensor))
    {
      Serial.print("InfluxDB write failed: ");
      Serial.println(client.getLastErrorMessage());
    }
  }
}

  static float Get_HumidityValue( void ) {
  uint16_t idx = 0u;
  uint32_t temp = 0u;
  for( idx=0; idx<SENSOR_BUFFER_SIZE; idx++ )
  {
    temp = temp + humidity_buffer[idx];
  }
  temp = temp/SENSOR_BUFFER_SIZE;
  return (float)temp;
  }


  static float Get_TemperatureValue( void ){
  uint16_t idx = 0u;
  uint32_t temp = 0u;
  for( idx=0; idx<SENSOR_BUFFER_SIZE; idx++ )
  {
    temp = temp + temp_buffer[idx];
  }
  temp = temp/SENSOR_BUFFER_SIZE;
  return (float)temp;
}
  