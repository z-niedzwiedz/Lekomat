# Lekomat
## Urządzenie do terminowego wydawania lekarstw, 2017

https://www.elektroda.pl/rtvforum/topic3594898.html

Wstęp
Lekomat - urządzenie do dozowania leków Lekomat - urządzenie do dozowania leków
Lekomat to urządzenie służące do wydawania osobie niedowidzącej leków o określonych godzinach.
Pomysł skonstruowania takiego urządzenia podrzuciła mi moja lepsza połówka a powstał z potrzeby chwili - senior będący pod naszą opieką dostał zalecenie otrzymywania leków w porze naszej nieobecności.
Testowaliśmy wcześniej inne rozwiązanie ale obsługa okazała się zbyt skomplikowana, przyciski małe i nieczytelne a sposób wydawania leków nieprzyjazny dla seniora z niepełnosprawnościami.

Opis konstrukcji
Obudowa powstała na bazie drewnianego pudełka do decoupage'u. Składa się z trzech części: boczne są komorami na zasobniki na leki zaś centralna zawiera elektronikę sterującą i elementy wykonawcze (serwomechanizmy odpowiedzialne za otwieranie pokryw komór). Aby to uzyskać przeciąłem pokrywę na trzy części, wzmocniłem je listewkami a do części bocznych dodałem elementy służące do naciskania wyłączników krańcowych. Wszystkie elementy drewniane polakierowałem lakierem bezbarwnym. Ślizgi i przedłużenia ramion serwomechanizmów wyciąłem z laminatu i połączyłem za pomocą sprężyn, zastosowałem nowe zawiasy. Do centralnej części pokrywy przymocowałem boczki wycięte z tworzywa sztucznego i wykorzystałem wewnętrzne rowki pudełka (które oryginalnie służyły jako prowadnice do drewnianych przekładek) aby zamocować centralny moduł na wcisk. Wywierciłem w niej otwory na przycisk oraz buzzer. W dolnej części pudełka wywierciłem otwory na diody sygnalizacyjne, enkoder obrotowy, zasilanie, kabel USB oraz nóżki. Wyciąłem w niej również otwór na wtyczkę taśmową IDC służącą jako złącze dla wyświetlacza, którego czarną obudowę wydrukowałem z PLA. W bocznych komorach umieściłem zielone moduły na zasobniki, również wydrukowane z PLA i wyposażone w naciskane przez kieliszki dźwignie przenoszące nacisk na krańcówki. Jako zasobników na leki użyłem kieliszków do wódki 25 ml firmy Krosno z serii Basic Glass.

Elektronika
Schemat i PCB:

Spis elementów:
- Arduino Nano 3.0
- 2 szt. serwo TowerPro MG-996R
- LCD5110/PCD8544
- Tiny RTC DS1307
- stabilizator 3,3V
- enkoder obrotowy
- buzzer
- 4 szt. LED
- tranzystor npn
- dioda prostownicza
- kondensator elektrolityczny
- oporniki
- podświetlany przycisk
- gniazdo zasilania
- 4 szt. wyłączniki krańcowe
- zasilacz 7,5V

Był to mój pierwszy projekt w środowisku Arduino (pierwsze moduły Uno i Nano kupiłem we wrześniu 2017), nie programowałem też wcześniej w C. Początkowo próbowałem użyć wersji Uno z keypadem i wyświetlaczem znakowym ale gabaryty tego zestawu powodowały, że nie mogłem znaleźć pasującej obudowy. Po sugestii siostry zakupiłem w Empiku pudełko i wiedziałem już, że muszę oszczędzać miejsce - stąd wykorzystanie Arduino Nano, enkodera obrotowego i wyświetlacza od Nokii. W trakcie testów okazało się też, że początkowo zastosowane serwomechanizmy SG90 są zbyt słabe - po zmianie na MG-996R w centralnej części zrobiło się jeszcze ciaśniej. Planowałem połączenie urządzenia z siecią WiFi ale okazało się, że w Nano jest za mało pamięci na moje pomysły i w końcu nawet nie zamontowałem modułu ESP-015.

W każdej z komór bocznych zamontowałem moduł odpowiadający z detekcję i sygnalizację: otwartej pokrywy oraz obecności zasobnika z lekami (po otwarciu pokrywy wnętrze podświetla jedna dioda a druga na zewnątrz sygnalizuje obecność kieliszka). Aby zaoszczędzić ilość pinów Arduino wykorzystywanych do komunikacji z modułem zastosowałem sztuczkę - LEDy o różnych kolorach oraz rezystory o różnych wartościach dające w rezultacie wyjściowy sygnał o kombinacji poziomów dających się stosunkowo łatwo wykryć na pinie analogowym.

Urządzenie zasilane jest napięciem 7,5V ze względu na zasilanie serwomechanizmów. Pozostała część pracuje pod napięciem 3,3V (zastosowałem osobny stabilizator). Układ zmontowałem na płytce uniwersalnej.

Na głównym ekranie urządzenia umieszczone są następujące informacje: aktualny czas, następne zdarzenie (godzina i strona podajnika) oraz statusy obu podajników. Naciśnięcie enkodera powoduje wejście do menu (po podaniu hasła). W menu można ustawiać i sprawdzać poszczególne elementy Lekomatu, otwierać i zamykać pokrywy itd. https://youtu.be/wj8uHGmYUVw

W momencie nadejścia zaprogramowanej godziny Lekomat zaczyna cyklicznie błyskać (wyświetlaczem oraz górnym przyciskiem) i piszczeć, należy wówczas nacisnąć górny przycisk - otwiera się wtedy odpowiednia pokrywa. Jeśli przez kilka sekund użytkownik nie wyjmie kieliszka - alarm rozlega się powtórnie. https://youtu.be/N1590mxnUck

Lekomat można załadować w prosty sposób: wkładając kieliszek do pustej komory a następnie naciskając górny przycisk. https://youtu.be/iEubPIlIa58

Oprogramowanie
Oczywiście używałem środowiska Arduino. Korzystałem też z edytora menu ale bibliotekę przerobiłem tak aby zamiast dwulinijkowego LCD obsługiwała LCD Nokii. Również obsługę enkodera musiałem zmienić ponieważ podczas normalnego użytkowania następował konflikt z serwomechanizmami (o ile pamiętam chodziło o programowanie timerów w Arduino Nano). Do tego RTClib oraz Adafruit_GFX i Adafruit_PCD8544.

Sporo korzystałem z programów przykładowych dostarczonych wraz z bibliotekami oraz filmów, głównie na kanale educ8s.

Podsumowanie
Myślę, że łączny koszt elementów i materiałów zużytych do zbudowania obecnej wersji Lekomatu wyniósł około 200 zł. Zadebiutował on "produkcyjnie" pod sam koniec 2017 roku i po drobnych poprawkach oprogramowania działa po dziś dzień. Jedyną jak na razie usterką (która wystąpiła po ok. roku działania) było uszkodzenie jednego z serwomechanizmów - po jego wymianie urządzenie działa bezproblemowo. Drobną uciążliwością jest niedokładność RTC - w ciągu miesiąca potrafi się rozregulować o kilka minut. Jak się okazało przestawianie zegara jest stosunkowo najczęściej używaną funkcją menu bo na co dzień nie ma potrzeby wchodzenia do niego - większość działań wykorzystuje górny przycisk i czujniki. W planach na niesprecyzowaną przyszłość mam wykonanie kolejnej wersji Lekomatu - tym razem synchronizującej czas z internetem i prezentującej rejestr zdarzeń za pomocą przeglądarki.
