#Testausdokumentti

##hsocket

Hsocket luokkaa on testattu avaamalla kaksi sockettia toinen vastaanottamaan ja toinen lähettämään. Näin saadaan testattu hsocket, luokan bind-, connect-, ja lähetys- ja vastaanottometodit.

Testejä on kaksi, joista toinen testaa blokkaavia socketteja ja toinen ei-blokkaavia socketteja.

##dns_packet

DNS-pakettiluokalle on kolme testiä. Ensimmäiset kaksi testiä testaavat pakettien muuttamista oliosta tavuvirraksi ja takaisin. Ensimmäisessä testataan vain kysymyksiä sisältävällä DNS-paketilla ja toisessa testataan kysymyksiä sekä vastauksia sisältävää DNS-pakettioliota.

Viimeisessä testissä testataan oikean DNS-kyselyn tekemistä Googlen palvelimelle osoitteessa 8.8.8.8. Testissä kysytään helsinki.fi.-nimen osoitetta. DNS-kysely muutetaan tavuiksi dns_packet::str()-metodilla ja lähetetään socketin kautta Googlelle. Vastaukseksi saatu tavuvirta muutetaan DNS-paketiksi ja tarkistetaan onko vastauksissa helsinki.fi:ä vastaavaa A-tyyppistä IP-osoitetta (IPv4). Mikäli vastauksen IP-osoite on oikein, testi onnistuu.

Huomion arvoista on, että DNS-luokan testaamisessa käytän aliluokkaa dns_test_t, jolle on määritelty DNS-headerin palauttava metodi, koska tavallisesti DNS-headeriin ei tarvitse koskea käyttäessa DNS-luokkaa.

##Testien ajaminen

Testit voidaan ajaa suorittamalla ```make``` tai ```make win``` komento src kansiossa riippuen siitä halutaanko kääntää ristiin windowsille vai kääntää natiivisti linuxille.

Jos halutaan ajaa vain testit se onnistuu ajamalla ```make tests```.

Huomioi, että ohjelmaa ei ole tarkoitettu käännettävän windowsissa vaan vain linuxissa make-ohjelmalla ja gcc:llä.
