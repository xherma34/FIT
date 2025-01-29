# IDS 2022 - projekt
### 1. Část
#### Autoři: 
* Pavel Heřmann - xherma34
* Alexander Sila - xsilaa00

### Stručný popis ER-diagramu
ER-diagram eviduje informace o personálu, pacientech, odděleních, hospitalizacích a léčivech.
Do množiny personálu spadají 3 synovské entity: Lékař, Sestra, Recepční. U každé/ho hospitalizace/vysetření musí být uveden lékař, který daný zákrok provedl, dále musí mít uvedeného příjmaného pacienta, pokud se pacient objednal u recepční, eviduje ho do systému lékař (aby nedošlo k situaci, kdy recepční zaeviduje objednané vyšetření a pacient se nedostaví).
Každé oddělení má jistý počet místností, dále je na něm vždy přidělen minimálně jeden doktor a jedna sestra.
Informační systém obsahuje databázy léčiv, které předepisuje lékař, dále musí být uvedeno která léčiva pacient užívá.