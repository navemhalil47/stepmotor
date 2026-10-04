#include <Stepper.h>

// --- Motor Ayarları ---
const int tamTurAdim = 2048; 
Stepper stepMotor(tamTurAdim, 8, 10, 9, 11);

// --- Sensör Ayarları ---
const int SES_PIN = A0;   
int ESIK_DEGERI = 80;    // Senin ayarladığın eşik değeri

// --- Çift Alkış Mantığı Değişkenleri ---
int alkisSayisi = 0;
unsigned long ilkAlkisZamani = 0;
unsigned long sonAlkisZamani = 0;

const int CIFT_ALKIS_PENCERESI = 1200; // İlk ve ikinci alkış arası maks 1.2 saniye olmalı
const int ALKIS_ARASI_MIN_SURE  = 300;  // İlk alkışın yankısını 2. alkış saymamak için min bekleme (ms)
const int MOTOR_BEKLEME        = 500; // Motor çalıştıktan sonraki soğuma süresi (ms)
unsigned long motorSonCalisma  = 0;

void setup() {
  Serial.begin(9600);
  stepMotor.setSpeed(7);
  Serial.println("Çift Alkış Sistemi Hazır! 1.2sn içinde 2 alkış bekleniyor...");
}

// 30 milisaniye boyunca en yüksek sesi yakalayan fonksiyon
int peakAl() {
  int peak = 0;
  unsigned long peakStart = millis();
  while (millis() - peakStart < 30) {
    int val = analogRead(SES_PIN);
    if (val > peak) peak = val;
  }
  return peak;
}

void loop() {
  // Motor yeni çalıştıysa soğuma süresince bekle
  if (millis() - motorSonCalisma < MOTOR_BEKLEME) {
    return;
  }

  int peak = peakAl();

  // --- Zaman Aşımı Kontrolü ---
  // İlk alkış alındı ama 1.2 saniye içinde ikinci alkış gelmediyse sıfırla
  if (alkisSayisi == 1 && (millis() - ilkAlkisZamani > CIFT_ALKIS_PENCERESI)) {
    alkisSayisi = 0;
    Serial.println("-> Zaman aşımı! 2. alkış gelmedi, sıfırlandı.");
  }

  // --- Alkış Yakalama ---
  if (peak > ESIK_DEGERI) {
    
    if (alkisSayisi == 0) {
      // BİRİNCİ ALKIŞ
      alkisSayisi = 1;
      ilkAlkisZamani = millis();
      sonAlkisZamani = millis();
      Serial.print("1. ALKIŞ YAKALANDI! (Peak: ");
      Serial.print(peak);
      Serial.println(") -> 2. Alkış bekleniyor...");
      delay(50); // Kısa bir es
    } 
    else if (alkisSayisi == 1 && (millis() - sonAlkisZamani > ALKIS_ARASI_MIN_SURE)) {
      // İKİNCİ ALKIŞ
      Serial.print("2. ALKIŞ YAKALANDI! (Peak: ");
      Serial.print(peak);
      Serial.println(") ---> MOTOR TETİKLENİYOR!");

      // Motor Hareket Alanı
      stepMotor.step(256);   // Sağa dön (Düğmeye bas)
      delay(500);            // Bekle
      stepMotor.step(-256);  // Sola dön (Eski konumuna gel)

      // Sistemi sıfırla
      alkisSayisi = 0;
      motorSonCalisma = millis();
      Serial.println("İşlem tamamlandı, sistem soğuma modunda...");
    }
  }
}
