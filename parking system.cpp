#include <iostream>
#include <vector>
#include <string>
#include <ctime>  // Zaman fonksiyonlari icin
#include <limits> // Giris hatalarini temizlemek icin (cin.ignore)

using namespace std;

// --- BASE CLASS (ANA SINIF) ---
class Vehicle {
protected:
    string licensePlate;
    string type;
    time_t entryTime; // Giris saati

public:
    Vehicle(string plate, string t) {
        licensePlate = plate;
        type = t;
        entryTime = time(0); // Nesne olusturuldugu an zamani kaydet
    }

    // Saf Sanal Fonksiyon (Alt siniflar doldurmak zorunda)
    virtual double calculateFee(int hours) = 0;

    string getLicensePlate() { return licensePlate; }
    string getType() { return type; }
    time_t getEntryTime() { return entryTime; }

    virtual ~Vehicle() {}
};

// --- DERIVED CLASSES (ALT SINIFLAR) ---

class Car : public Vehicle {
public:
    Car(string plate) : Vehicle(plate, "Car") {}
    double calculateFee(int hours) { return hours * 20.0; }
};

class Motorbike : public Vehicle {
public:
    Motorbike(string plate) : Vehicle(plate, "Motorbike") {}
    double calculateFee(int hours) { return hours * 10.0; }
};

class Truck : public Vehicle {
public:
    Truck(string plate) : Vehicle(plate, "Truck") {}
    double calculateFee(int hours) { return hours * 40.0; }
};

// --- PARKING LOT CLASS (OTOPARK YONETIMI) ---
class ParkingLot {
private:
    vector<Vehicle*> parkedVehicles;
    int capacity;

public:
    ParkingLot(int cap) {
        capacity = cap;
    }

    // Otopark dolu mu kontrolu
    bool isFull() {
        return parkedVehicles.size() >= capacity;
    }

    // YENI EKLENEN: Ayni plaka iceride var mi kontrolu
    bool isPlateExists(string plateToCheck) {
        for (size_t i = 0; i < parkedVehicles.size(); i++) {
            if (parkedVehicles[i]->getLicensePlate() == plateToCheck) {
                return true; // Eger plaka bulunduysa true dondur
            }
        }
        return false; // Bulunamadiysa false
    }

    void parkVehicle(Vehicle* vehicle) {
        if (parkedVehicles.size() >= capacity) {
            cout << "HATA: Otopark dolu!" << endl;
            delete vehicle; // Yer yoksa nesneyi iptal et
        } else {
            parkedVehicles.push_back(vehicle);
            
            // Giris saatini ekrana yazdirma
            time_t tempEntry = vehicle->getEntryTime(); 
            cout << "Arac Giris Yapti: " << vehicle->getLicensePlate() << endl;
            cout << "Giris Zamani: " << ctime(&tempEntry); 
        }
    }

    void leaveVehicle(string plateToFind) {
        bool found = false;
        for (size_t i = 0; i < parkedVehicles.size(); i++) {
            if (parkedVehicles[i]->getLicensePlate() == plateToFind) {
                found = true;
                
                // Cikis islemleri
                time_t exitTime = time(0); // Su anki zaman
                
                // Fark hesabi (Saniye cinsinden)
                double secondsPassed = difftime(exitTime, parkedVehicles[i]->getEntryTime());
                
                // SIMULASYON: 1 Saniye = 1 Saat kabul ediyoruz
                int hours = (int)secondsPassed;
                if(hours == 0) hours = 1; // En az 1 saat ucreti

                // Ucret hesapla (Polymorphism)
                double fee = parkedVehicles[i]->calculateFee(hours);
                
                time_t tempEntry = parkedVehicles[i]->getEntryTime();

                cout << "----------------------------" << endl;
                cout << "     OTOMATIK FIS DETAYI    " << endl;
                cout << "----------------------------" << endl;
                cout << "Plaka    : " << parkedVehicles[i]->getLicensePlate() << endl;
                cout << "Tur      : " << parkedVehicles[i]->getType() << endl;
                cout << "Giris    : " << ctime(&tempEntry); 
                cout << "Cikis    : " << ctime(&exitTime);
                cout << "Sure     : " << hours << " Saat (Simulasyon: 1 sn = 1 saat)" << endl;
                cout << "TUTAR    : " << fee << " TL" << endl;
                cout << "----------------------------" << endl;

                // Hafiza temizligi
                delete parkedVehicles[i];
                parkedVehicles.erase(parkedVehicles.begin() + i);
                cout << "Arac cikisi ve odeme basarili." << endl;
                break;
            }
        }
        if (!found) {
            cout << "HATA: " << plateToFind << " plakali arac bulunamadi!" << endl;
        }
    }

    void displayStatus() {
        cout << "\n--- OTOPARK DURUMU ---" << endl;
        cout << "Kapasite: " << capacity << endl;
        cout << "Dolu    : " << parkedVehicles.size() << endl;
        cout << "Bos     : " << capacity - parkedVehicles.size() << endl;
        
        if (parkedVehicles.size() > 0) {
            cout << "Park Halindeki Araclar:" << endl;
            for (size_t i = 0; i < parkedVehicles.size(); i++) {
                time_t tempEntry = parkedVehicles[i]->getEntryTime();
                string zamanStr = ctime(&tempEntry);
                
                if (!zamanStr.empty() && zamanStr[zamanStr.length()-1] == '\n') {
                    zamanStr.erase(zamanStr.length()-1);
                }

                cout << i+1 << ". " << parkedVehicles[i]->getLicensePlate() 
                     << " [" << parkedVehicles[i]->getType() << "] - Giris: " << zamanStr << endl;
            }
        }
        cout << "----------------------" << endl;
    }
};

// --- MAIN (ANA PROGRAM) ---
int main() {
    ParkingLot myLot(5); // 5 araclik otopark
    int choice;

    while (true) {
        cout << "\n--- OTOPARK OTOMASYONU ---" << endl;
        cout << "1. Arac Giris" << endl;
        cout << "2. Arac Cikis (Otomatik Odeme)" << endl;
        cout << "3. Durum Goster" << endl;
        cout << "0. Cikis" << endl;
        cout << "Secim: ";
        
        // --- INPUT VALIDATION (GIRIS KONTROLU) ---
        // Kullanici harf girerse cin hata verir, bunu yakaliyoruz
        if (!(cin >> choice)) {
            cout << "HATA: Lutfen gecerli bir SAYI giriniz!" << endl;
            cin.clear(); // Hata bayragini temizle
            cin.ignore(10000, '\n'); // Tampondaki hatali girisi (harfleri) sil
            continue; // Dongu basina don
        }

        if (choice == 0) break;

        if (choice == 1) {
            if (myLot.isFull()) {
                cout << "UYARI: Otopark tamamen dolu! Giris yapilamaz." << endl;
                continue; 
            }

            string plate;
            int typeChoice;
            cout << "Plaka Giriniz: ";
            cin >> plate;

            // --- YENI: AYNI PLAKA KONTROLU ---
            if (myLot.isPlateExists(plate)) {
                cout << "HATA: " << plate << " plakali arac zaten otoparkta!" << endl;
                continue; // Menuye don
            }
            
            cout << "Tur Sec (1-Araba, 2-Motor, 3-Kamyon): ";
            
            // --- INPUT VALIDATION (TUR SECIMI ICIN) ---
            if (!(cin >> typeChoice)) {
                cout << "HATA: Arac turu icin sayi (1-3) girmelisiniz!" << endl;
                cin.clear();
                cin.ignore(10000, '\n');
                continue;
            }

            Vehicle* v = NULL;

            if (typeChoice == 1)      v = new Car(plate);
            else if (typeChoice == 2) v = new Motorbike(plate);
            else if (typeChoice == 3) v = new Truck(plate);
            else cout << "Gecersiz tur secimi!" << endl;

            if (v != NULL) {
                myLot.parkVehicle(v);
            }

        } else if (choice == 2) {
            string plate;
            cout << "Cikis Yapacak Plaka: ";
            cin >> plate;
            myLot.leaveVehicle(plate);

        } else if (choice == 3) {
            myLot.displayStatus();
        } else {
            cout << "Gecersiz islem! Lutfen 0-3 arasi secim yapin." << endl;
        }
    }

    return 0;
}