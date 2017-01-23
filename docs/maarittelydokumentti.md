#Aiheen määrittely

Työssäni tulen käyttämään ainakin linkitettyä listaa, jonoa ja todennäköisesti myös kekoa. Algoritmeja työhön tulee muun muassa osittainen base64-tyyppinen koodaus datalle ja asynkronisen I/O:n toteutus viestijonolla.

Työni tulee olemaan C++:lla toteutettu kaksisuuntainen DNS data tunneli, joka koostuu palvelimesta ja asiakkaasta. Tunnelin asiakas-ohjelma ottaa yhtettä palvelimeen, joka kuuntelee UDP-portissa DNS-kyselyitä. Sekä palvelin, että asiakas voivat ottaa mitä tahansa syötettä vastaan standard inputtiin ja se siirretään vastaavasti toisella puolella olevan palvelimen tai asiakkaan standard outputtiin.

Tunneli siis toimii molempiin suuntiin ja asiakas ja palvelin viittaavat tässä tapauksessa vain kuuntelevaan ja yhteyttä ottavaan osapuoleen. Silloin, kun tunnelissa ei kulje aktiivisesti dataa lähettää asiakas silti palvelimelle jatkuvasti heartbeat viestejä kertoen olemassaolostaan. Palvelin ei nimittäin voi lähettää dataa asiakkalle muuta kuin DNS-vastauksien kautta.

Asiakasohjelmassa kaikki data koodataan ascii merkeiksi DNS-kyselyyn sopivaan muotoon ja muutetaan DNS-vastaukseksi, joka koodataan tavuiksi muotoon, jonka voi lähettää UDP:lla palvelimelle. Palvelimessa dataa ei tarvitse koodata ascii-merkeiksi, sillä DNS-vastauksissa palautetaan IP:itä tavumuodossa. DNS-vastauksissa voi myös olla useita vastauksia samaan kysymykseen. Käytän todennäköisesti ensin vain yhtä kysymystyyppiä "A", joka on siis kysymys, johon vastataan IPv4-osoitteilla. Palvelin siis jakaa lähetettävän datapaketin neljän tavun lohkoihin ja lisää sen DNS-vastaukseen.

Datapaketit vaativat oman protokollan, jotta kommunikaatio voi olla kaksisuuntaista. Datapaketit tulevat siis koostumaan paketin pituudesta, operaatiokoodista ja itse datasta. Operaatiokoodit tulevat olemaan esimerkiksi, HEARTBEAT, OK, DATA, tms.

Datapaketit koodataan siis DNS-kyselyihin ja vastauksiin ja DNS toimii vain transport-protokollana.

Aika- ja tilavaativuudet ovat vielä hieman hämärän peitossa, mutta DNS-kyselyitä parsitaan yksi kerrallaan, ja ne ovat vakiopituisia (max 4096 tavua). Muusta datan käsittelystä tulee todennäköisesti enemmän vaatimuksia.

Lähteenä käytän DNS-kyselyiden osalta http://www.tcpipguide.com/free/t_DNSMessageHeaderandQuestionSectionFormat.htm, muissa kysymyksissä käytän stackoverflow-foorumia.
