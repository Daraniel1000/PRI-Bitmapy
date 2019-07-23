# Projekt 3.3: Obróbka zdjęć w formacie BMP
Treść:
Napisać program do obróbki cyfrowej bitmap. Program powinien realizować następujące funkcjonalności:
•	Wczytywanie do pamięci bitmapy o dowolnym nieustalonym z góry rozmiarze (powinna być ona przechowywana jako dynamiczna tablica struktur)
•	Odszumianie wczytanego obrazka (w skali szarości bądź kolorowego 24, 32, lub 16-bitowego) za pomocą filtrów: medianowego, uśredniającego, minimalnego i maksymalnego o różnych rozmiarach okna (np.: 3x3, 10x10), które mogą być zdefiniowane przez użytkownika
•	Konwersja obrazu kolorowego (32, 24, 16, 8-bitowego) na obraz w skali szarości
•	Generowanie negatywu wczytanej bitmapy 
•	Zapis przekonwertowanych obrazków do oddzielnych plików z rozszerzeniem BMP

Uwagi do programu końcowego:
Program nie działa dla innych niż 24-bitowych bitmap.
Największa bitmapa jaką program jest w stanie wczytać ze względów pamięciowych ma ok 650000 pikseli.
