#Toteutusdokumentti

##Ohjelman rakenne

Ohjelma koostuu kahdesta erillisestä ohjelmasta. Palvelimesta ja asiakkaasta. Palvelin kuuntelee DNS-viestejä ja asiakas lähettää niitä.

Tällä hetkellä palvelin ja asiakas on molemmat kovakoodattu lähettämään ja vastaanottamaan viestejä portissa 53 vain paikallisesti osoitteessa 127.0.0.1.

Ohjelmat kommunikoivat lähettämällä kahdentyyppisiä viestejä HEARTBEAT ja OK. HEARTBEAT viestit eivät sisällä dataa. Ne kertovat vain, että yhteys on vielä elossa ja mahdollistavat esimerkiksi palvelimen kommunikoinnin asiakkaalle, vaikka asiakkaalla ei olisi mitään dataa lähetettävänä. OK-tyyppiset viestit sisältävät datan, joka halutaan kuljettaa tunnelin läpi.

## protokollat, paketit ja datan koodaus

Kaikki protokolla-objektit on kuvattu objektien tavuesityksinä, jota tarvitaan niiden kuljettamiseen verkon yli.

Message-objekti, jota käytetään kommunikointiin on rakenteeltaan seuraavanlainen:

|8 bit|     16 bit  | n bit|
----------------------------
|type | data length | data |
----------------------------


DNS-objekti on taas rakenteeltaan seuraavanlainen:

|12 bytes |<= 25 bytes| n bytes   | n bytes     | n bytes     |
--------------------------------------------------------------
| header  | questions | responses | authorities | additionals |
--------------------------------------------------------------

Questions-osio, sisältää suurimmassa osassa tapauksia yhden kysymyksen. Responses-osio sisältää n-määrän vastauksia tavallisessa. Tavallisessa vastauksessa ei yleensä ole authority-osita, eikä additionals-osioita.

Questions-osion kysymykset ovat kysymysmuotoa. Responses-, authorities- ja additionals-osiot ovat taas kaikki samaa vastausmuotoa.

Kysymys- ja vastausmuodoista voi lukea lisätietoja täältä http://www.zytrax.com/books/dns/ch15/.

Aina, kun halutaan siirtää tietoa asiakkaalta palvelimelle tai palvelimelta asiakkaalle, tai molemmille samanaikaisesti, täytyy asiakkaan lähettää DNS-kysely ja palvelimen vastata siihen DNS-vastauksella. Tämän takia, vaikka dataa ei tarvisi kuljettaa juuri tietyllä hetkellä, lähetetään silti palvelimen ja asiakkaan välillä HEARTBEAT-viestejä.

Yhden viestiobjektin lähetys toimii seuraavasti:

Viestiobjekti muutetaan ensin tavuiksi, jotka lisätään uloskulkevaan puskuriin. Sen jälkeen puskurista otetaan sopiva pala, joka mahtuu yhteen DNS-kyselyyn. Asiakkaalla se tarkoittaa sitä, että datan täytyy sopia domain-nimeen ja palvelimella johonkin ennaltamääritettyyn määrään IP-osoitteita, kun käytetään A-tyypin vastauksia ja kyselyitä.

Kun DNS-kyselyn data dekoodataan takaisin tavuvirraksi toisessa päässä, se lisätään sisääntulevaan puskuriin. Kun puskurissa on tarpeeksi dataa vastaanotetun message-paketin muodostamiseksi, se muutetaan tavuista taas messageksi. Messagen tyypistä riippuen ei tehdä mitään (HEARTBEAT) tai tulostetaan saatu data (OK).

