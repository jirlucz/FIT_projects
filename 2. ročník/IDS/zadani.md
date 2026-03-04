# Projekt: Návrh a implementace relační databáze (Oracle 12c)

## Základní informace
* **Cíl:** Návrh a implementace relační databáze na zvolené téma.
* **Tým:** Dvoučlenný (možnost pokračovat v projektu z IUS nebo výběr ze seznamu témat).
* **Platforma:** SŘBD Oracle 12c.
* **Identifikace:** Název tématu musí být uveden ve všech výstupech (komentáře v SQL, diagramy, dokumentace).

## Obecné technické požadavky
* **Opakovatelnost:** SQL skripty musí umožňovat opakované spuštění (obsahovat příkazy pro odstranění a opětovné vytvoření objektů).
* **Kódování:** Textové soubory v kódování **UTF-8**.
* **Formát odevzdání:** Diagramy v **PDF**, SQL skripty jako prostý text.

---

## 1. část – Datový model a Model případů užití
Cílem je zachytit strukturu dat a funkční požadavky.

* **ER Diagram (ERD):**
    * Notace: UML Class Diagram nebo Crow's Foot.
    * **Povinnost:** Obsahovat alespoň jeden vztah **generalizace/specializace** (nadtyp/podtyp) se správnou notací.
* **Use Case Diagram:**
    * Notace: UML diagram případů užití.
    * Reprezentace funkcionality aplikace nad databází.
* **Dokumentace:** Stručný popis entit a vztahových množin.

---

## 2. část – SQL skript pro vytvoření objektů schématu
Implementace základní struktury a integritních omezení.

* **Tabulky:** Definice PK (primární klíče) a FK (cizí klíče).
* **Speciální omezení:** Alespoň jeden sloupec s `CHECK` omezením pro validaci formátu (např. RČ, IČO, IBAN, ISBN).
* **Generalizace/Specializace:** Realizace vztahu v relačním modelu (v komentáři zdůvodnit zvolený způsob převodu).
* **Automatizace PK:** Generování hodnot primárního klíče ze **sekvence** (např. při vložení `NULL`).
* **Data:** Naplnění tabulek ukázkovými daty.

---

## 3. část – SQL skript s dotazy SELECT
Skript musí obsahovat vytvoření schématu, naplnění daty a minimálně **7 specifických dotazů**:

1. **2x** spojení dvou tabulek.
2. **1x** spojení tří tabulek.
3. **2x** klauzule `GROUP BY` s agregační funkcí.
4. **1x** predikát `EXISTS`.
5. **1x** predikát `IN` s vnořeným selectem.

> [!IMPORTANT]
> Každý dotaz musí mít komentář vysvětlující jeho srozumitelnou funkci v aplikaci.

---

## 4. část – Pokročilé objekty a optimalizace
Skript demonstrující pokročilé funkce SŘBD Oracle.

### Povinný obsah:
* **Triggery:** 2x netriviální databázový trigger (včetně předvedení funkčnosti).
* **Procedury:** 2x netriviální uložená procedura. Musí obsahovat:
    * Kurzor, ošetření výjimek a použití `%TYPE` nebo `%ROWTYPE`.
* **Indexy & EXPLAIN PLAN:**
    * Vytvoření indexu pro optimalizaci konkrétního dotazu.
    * Demonstrace pomocí `EXPLAIN PLAN` (před a po optimalizaci).
    * Dotaz pro EXPLAIN PLAN musí obsahovat spojení 2 tabulek, `GROUP BY` a agregaci.
* **Přístupová práva:** Definice oprávnění pro druhého člena týmu.
* **Materializovaný pohled:** Vytvoření pohledu patřícího druhému členu týmu nad tabulkami prvního člena.
* **Komplexní SELECT:** Využití klauzule `WITH` a operátoru `CASE`.
