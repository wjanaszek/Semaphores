# Semaphores

Aplikacja realizująca dostęp 3 procesów do wspólnych zasobów (buforów).
- proces A1 (plik proc_A1.c) produkuje parzyste liczby do bufora B1, dopóki nie ma w nim X elementów
- proces A2 (plik proc_A2.c) pobiera liczby z bufora B1 i wkłada o jeden większe do B2, dopóki w B2 jest mniej niż Y elementów
- proces A3 (plik proc_A3.c) pobiera liczby z bufora B2 (dopóki jest w nim conajmniej Z elementów), i jeśli pobrana liczba jest podzielna przez 3, to wkłada ją do bufora B1
- plik main.c inicjalizuje wszystkie potrzebne segmenty współdzielonej pamięci, semafory; zapewnia też wypisywanie co sekundę 
rozmiaru i zawartości buforów

Testowanie polega na uruchomieniu pliku main z argumentem oznaczającym maksymalną pojemność bufora, a następnie dodawanie (np. w drugim
oknie terminalu) kolejnych procesów, uruchamiając pliki proc_A1, proc_A2, proc_A3. 
