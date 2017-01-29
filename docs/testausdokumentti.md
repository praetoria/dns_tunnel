#Testausdokumentti

##hsocket

Hsocket luokkaa on testattu avaamalla kaksi sockettia toinen vastaanottamaan ja toinen lähettämaan. Näin saadaan testattu hsocket, luokan bind-, connect-, ja lähetys- ja vastaanottometodit.

Testejä on kaksi, joista toinen testaa blokkaavia socketteja ja toinen ei-blokkaavia socketteja.

##Testien ajaminen

Testit voidaan ajaa suorittamalla ```make``` tai ```make win``` komento src kansiossa riippuen siitä halutaanko kääntää ristiin windowsille vai kääntää natiivisti linuxille.

Jos halutaan ajaa vain testit se onnistuu ajamalla ```make tests```.

Huomioi, että ohjelmaa ei ole tarkoitettu käännettävän windowsissa vaan vain linuxissa make-ohjelmalla.
