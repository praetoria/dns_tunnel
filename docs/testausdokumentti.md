#Testausdokumentti

##integraatiotestaus

Integraatiotestausta en ole automatisoinut. Testasin manuaalisesti palvelin- ja asiakasohjelmia lähettämällä testiviestejä ja tiedostoja kumpaankin suuntaan. Samalla ajoin tcpdumppia, jolla tarkistin, että DNS-paketit näyttävät speksin mukaiselta ja niissä on data koodattu oikein.

##hsocket

Hsocket-luokkaa on testattu avaamalla kaksi sockettia toinen vastaanottamaan ja toinen lähettämään. Näin saadaan testattu hsocket, luokan bind-, connect-, ja lähetys- ja vastaanottometodit.

Testejä on kaksi, joista toinen testaa blokkaavia socketteja ja toinen ei-blokkaavia socketteja.

##dns_packet

DNS-pakettiluokalle on kolme testiä. Ensimmäiset kaksi testiä testaavat pakettien muuttamista oliosta tavuvirraksi ja takaisin. Ensimmäisessä testataan vain kysymyksiä sisältävällä DNS-paketilla ja toisessa testataan kysymyksiä sekä vastauksia sisältävää DNS-pakettioliota.

Viimeisessä testissä testataan oikean DNS-kyselyn tekemistä Googlen palvelimelle osoitteessa 8.8.8.8. Testissä kysytään helsinki.fi.-nimen osoitetta. DNS-kysely muutetaan tavuiksi dns_packet::str()-metodilla ja lähetetään socketin kautta Googlelle. Vastaukseksi saatu tavuvirta muutetaan DNS-paketiksi ja tarkistetaan onko vastauksissa helsinki.fi:ä vastaavaa A-tyyppistä IP-osoitetta (IPv4). Mikäli vastauksen IP-osoite on oikein, testi onnistuu.

Huomion arvoista on, että DNS-luokan testaamisessa käytän aliluokkaa dns_test_t, jolle on määritelty DNS-headerin palauttava metodi, koska tavallisesti DNS-headeriin ei tarvitse koskea käyttäessa DNS-luokkaa.

##message

Message-luokalle on yksi testi. Testi testaa message-luokan olioiden muuttamista tavuvirraksi ja takaisin, mikä on message-luokan ainoa haluttu toiminnallisuus.

Testi testaa ensin HEARTBEAT-tyyppistä viestiä, sitten OK-tyyppistä viestiä ja lopuksi M_ERROR-tyyppistä viestiä, jonka rakentamista tavuista ei tueta, joten tuloksen pitää olla viimeisessä tilanteessa eri alkuperäisestä (ei dataa).

M_ERROR-tyyppiä ei käytetä muuhun kuin virheellisen viestin tarkistukseen.

##convert_util

Convert_util on moduuli, joka sisältää apufunktioita, joilla voidaan muuttaa dataa heksamuotoon ja takaisin.

Moduulille on yksi testi, joka testaa, että, kun kutsutaan datalla to_hex()-funktiota ja sen tuloksella from_hex()-funktiota, saadaan takaisin alkuperäinen data.

##tunnel_dns

Tunnel_dns-luokkaa testataan kolmella testillä. DNS-tunneliobjekteja on tällä hetkellä INCOMING- ja OUTGOING-tyyppisiä, jonka lisäksi ne ovat joko kysely-tyyppiä tai vastaus-tyyppiä. Vain A-tyyppisille tallenteille on toteutettu tuki.

Ensimmäinen testi testaa, että OUTGOING-tyyppiseen DNS-kyselytunneliin laitettu data koodautuu oikein domain-nimeksi.

Toinen testi testaa, että OUTGOING-tyyppisestä DNS-kyselytunnelista saatu data koodautuu takaisin dataksi, kun se laitaan INCOMING-tyyppisen DNS-kyselytunnelin läpi.

Kolmas testi testaa, että OUTGOING-tyyppisestä DNS-vastaustunnelista saatu data kodautuu oikein IP-osoitteiksi. IP-osoitteita on monta, sillä yksi DNS-vastauspaketti voi sisältää monta IP-osoitetta vastauksena kysytylle domain-nimelle.

##pque

Pque-luokkaa testataan lisäämällä sinne std::string-objekteja vectorista epäjärjestyksessä. Sen jälkeen sieltä poistetaan kaikki alkiot ja lisätään uuteen vektoriin. Vanha vektori järjestetään std::sort-funktiolla ja tulosvektoria verrataan vanhaan. Jos vektorit ovat identtisiä, testi menee läpi.
Testissä tulee testatuksi, size(), insert(), remove() ja pop() -operaatiot pque-luokasta.

##vec

Vec-luokkaa testataan lisäämällä sinne std::string objekteja ja sen jälkeen vertaamalla sen sisältöä ja kokoa vastaavaan std::vectoriin. Lisäksi vec-instanssi alustetaan yhden kokoiseksi, jolloin myös resize-ominaisuus tulee testattua.

##Testien ajaminen

Testit voidaan ajaa suorittamalla ```make``` tai ```make win``` komento src kansiossa riippuen siitä halutaanko kääntää ristiin windowsille vai kääntää natiivisti linuxille.

Jos halutaan ajaa vain testit se onnistuu ajamalla ```make tests```.

Huomioi, että ohjelmaa ei ole tarkoitettu käännettävän windowsissa vaan vain linuxissa make-ohjelmalla ja gcc:llä.
