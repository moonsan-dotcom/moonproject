#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD I2C
LiquidCrystal_I2C lcd(0x27, 16, 2);

// PIN INPUT SAKELAR
const int pinA = 2;
const int pinB = 3;

// PIN PUSH BUTTON
const int pinUP = 4;
const int pinDOWN = 5;
const int pinSELECT = 6;
const int pinBACK = 7;

// PIN LED
const int ledHijau = 10;
const int ledMerah = 11;

// Daftar tombol
const int tombol[4] = {
  pinUP, pinDOWN, pinSELECT, pinBACK
};

// Sistem debounce tombol
bool kondisiStabil[4] = {HIGH, HIGH, HIGH, HIGH};
bool pembacaanSebelumnya[4] = {HIGH, HIGH, HIGH, HIGH};
unsigned long waktuPerubahan[4] = {0, 0, 0, 0};

const unsigned long debounceDelay = 40;

// HALAMAN PROGRAM
int halaman = 0;

// PILIHAN KATEGORI
int kategori = 0;

// PILIHAN GERBANG
int pilihanGate = 0;

// INPUT DAN OUTPUT
int A, B, Y;

// JUMLAH GERBANG
const int jumlahDasar = 7;
const int jumlahKombinasi = 4;

// NAMA GERBANG DASAR
const char* namaDasar[] = {
  "AND",
  "OR",
  "NOT",
  "NAND",
  "NOR",
  "XOR",
  "XNOR"
};

// NAMA GERBANG KOMBINASI
const char* namaKombinasi[] = {
  "AND-NOT",
  "NOT-AND",
  "OR-NOT",
  "NOT-OR"
};


// =================================
// MEMBACA PUSH BUTTON
// =================================

bool tombolDitekan(int index) {

  bool pembacaan = digitalRead(tombol[index]);

  if (pembacaan != pembacaanSebelumnya[index]) {
    waktuPerubahan[index] = millis();
    pembacaanSebelumnya[index] = pembacaan;
  }

  if (millis() - waktuPerubahan[index] > debounceDelay) {

    if (pembacaan != kondisiStabil[index]) {

      kondisiStabil[index] = pembacaan;

      if (kondisiStabil[index] == LOW) {
        return true;
      }
    }
  }

  return false;
}


// =================================
// MEMBACA INPUT SAKELAR
// =================================

void bacaInput() {

  A = (digitalRead(pinA) == LOW) ? 1 : 0;
  B = (digitalRead(pinB) == LOW) ? 1 : 0;

}


// =================================
// PERHITUNGAN GERBANG DASAR
// =================================

int hitungDasar(int nomor, int a, int b) {

  switch (nomor) {

    case 0:
      return a && b;

    case 1:
      return a || b;

    case 2:
      return !a;

    case 3:
      return !(a && b);

    case 4:
      return !(a || b);

    case 5:
      return a ^ b;

    case 6:
      return !(a ^ b);
  }

  return 0;
}


// =================================
// PERHITUNGAN LOGIKA KOMBINASI
// =================================

int hitungKombinasi(int nomor, int a, int b) {

  switch (nomor) {

    case 0:
      return a && !b;

    case 1:
      return !a && b;

    case 2:
      return a || !b;

    case 3:
      return !a || b;
  }

  return 0;
}


// =================================
// TAMPILAN MENU UTAMA
// =================================

void tampilMenuUtama() {

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("PILIH KATEGORI");

  lcd.setCursor(0, 1);

  if (kategori == 0) {
    lcd.print("> BASIC GATE");
  }
  else {
    lcd.print("> KOMBINASI");
  }
}


// =================================
// TAMPILAN PILIHAN GERBANG
// =================================

void tampilMenuGate() {

  lcd.clear();

  if (kategori == 0) {

    lcd.setCursor(0, 0);
    lcd.print("GERBANG DASAR");

    lcd.setCursor(0, 1);
    lcd.print("> ");
    lcd.print(namaDasar[pilihanGate]);

  }

  else {

    lcd.setCursor(0, 0);
    lcd.print("LOGIKA KOMBINASI");

    lcd.setCursor(0, 1);
    lcd.print("> ");
    lcd.print(namaKombinasi[pilihanGate]);

  }
}


// =================================
// TAMPILAN HASIL PENGUJIAN
// =================================

void tampilHasil() {

  bacaInput();

  if (kategori == 0) {
    Y = hitungDasar(pilihanGate, A, B);
  }

  else {
    Y = hitungKombinasi(pilihanGate, A, B);
  }

  // LED INDIKATOR
  digitalWrite(ledHijau, Y == 1 ? HIGH : LOW);
  digitalWrite(ledMerah, Y == 0 ? HIGH : LOW);

  // LCD BARIS PERTAMA
  lcd.setCursor(0, 0);

  if (kategori == 0) {
    lcd.print(namaDasar[pilihanGate]);
  }

  else {
    lcd.print(namaKombinasi[pilihanGate]);
  }

  lcd.print("            ");

  // LCD BARIS KEDUA
  lcd.setCursor(0, 1);

  lcd.print("I1:");
  lcd.print(A);

  lcd.print(" I2:");
  lcd.print(B);

  lcd.print(" Out:");
  lcd.print(Y);

  lcd.print("     ");
}


// =================================
// SETUP
// =================================

void setup() {

  // INPUT SAKELAR
  pinMode(pinA, INPUT_PULLUP);
  pinMode(pinB, INPUT_PULLUP);

  // INPUT PUSH BUTTON
  for (int i = 0; i < 4; i++) {
    pinMode(tombol[i], INPUT_PULLUP);
  }

  // OUTPUT LED
  pinMode(ledHijau, OUTPUT);
  pinMode(ledMerah, OUTPUT);

  digitalWrite(ledHijau, LOW);
  digitalWrite(ledMerah, LOW);

  // LCD
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("TRAINER LOGIKA");

  lcd.setCursor(0, 1);
  lcd.print("ARDUINO UNO");

  delay(2000);

  tampilMenuUtama();
}


// =================================
// LOOP
// =================================

void loop() {

  // BACA SEMUA TOMBOL SETIAP LOOP
  bool up = tombolDitekan(0);
  bool down = tombolDitekan(1);
  bool select = tombolDitekan(2);
  bool back = tombolDitekan(3);


  // =================================
  // HALAMAN 0: MENU UTAMA
  // =================================

  if (halaman == 0) {

    if (up || down) {

      kategori = !kategori;

      tampilMenuUtama();
    }

    if (select) {

      halaman = 1;
      pilihanGate = 0;

      tampilMenuGate();
    }

  }


  // =================================
  // HALAMAN 1: PILIH GERBANG
  // =================================

  else if (halaman == 1) {

    int jumlahGate;

    if (kategori == 0) {
      jumlahGate = jumlahDasar;
    }
    else {
      jumlahGate = jumlahKombinasi;
    }

    if (up) {

      pilihanGate--;

      if (pilihanGate < 0) {
        pilihanGate = jumlahGate - 1;
      }

      tampilMenuGate();
    }

    if (down) {

      pilihanGate++;

      if (pilihanGate >= jumlahGate) {
        pilihanGate = 0;
      }

      tampilMenuGate();
    }

    if (select) {

      halaman = 2;

      lcd.clear();

      tampilHasil();
    }

    if (back) {

      halaman = 0;

      tampilMenuUtama();
    }

  }


  // =================================
  // HALAMAN 2: PENGUJIAN GERBANG
  // =================================

  else if (halaman == 2) {

    static unsigned long waktuUpdate = 0;

    if (millis() - waktuUpdate >= 100) {

      waktuUpdate = millis();

      tampilHasil();
    }

    if (back) {

      halaman = 1;

      digitalWrite(ledHijau, LOW);
      digitalWrite(ledMerah, LOW);

      tampilMenuGate();
    }

  }

}