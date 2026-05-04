void SURE() {
  genCLS = 1; // Effacer l’écran

  setback(); // Dessiner l’arrière-plan
  getprev(); // Récupérer les 3 blocs les plus à droite de l’écran de gauche

  SCRNUM = calcblue(); // Calculer l’adresse mémoire de l’écran

  // **Dessiner les 3 lignes de 10 bloc²s (de gauche à droite, de haut en bas)**
  for (rowno = 2; rowno >= 0; rowno--) {
    Dy = BlockBot[rowno + 1]; // Coordonnée Y de la section D
    Ay = Dy - 3;              // Coordonnée Y de la section A

    yindex = Mult10[rowno]; // Numéro de bloc (0-29)
    PRECED = PREV[rowno];
    spreced = sprev[rowno]; // Récupérer les infos du bloc précédent

    getbelow(); // Récupérer les 10 blocs du dessous

    // **Parcourir chaque colonne de la ligne**
    for (colno = 0; colno < 10; colno++) {
      XCO = blockxco = colno * 4; // Calculer la position X

      objid = getobjid(); // Récupérer l'ID de l'objet
      RedBlockSure();     // Redessiner le bloc entier

      PRECED = objid;
      spreced = state; // Passer au bloc suivant

      yindex++; // Incrémenter le compteur de blocs
    }
  }

  // **Dessiner la ligne du bas de l’écran au-dessus (sections D uniquement)**
  rowno = 2;
  Dy = 2;
  Ay = -1;
  yindex = Mult10[rowno];
  PRECED = 0;

  // **Sauvegarder les valeurs de l’écran en dessous**
  uint8_t tempScrnBelow = scrnBelow;
  uint8_t tempScrnBelowL = scrnBelowL;

  scrnBelow = SCRNUM;
  scrnBelowL = scrnLeft; // Simuler qu'on est sur l'écran au-dessus

  getbelow();
  scrnAbove = calcblue();

  // **Dessiner les 10 blocs de gauche à droite**
  for (colno = 0; colno < 10; colno++) {
    XCO = blockxco = colno * 4;

    if (scrnAbove == 0) {
      objid = FLOOR; // Si l'écran au-dessus est nul, on dessine un sol plein
    } else {
      objid = getobjid();
    }

    RedDSure(); // Dessiner uniquement la section D

    PRECED = objid;
    spreced = state;

    yindex++; // Incrémenter le compteur de blocs
  }

  // **Restaurer les valeurs originales de l’écran en dessous**
  scrnBelowL = tempScrnBelowL;
  scrnBelow = tempScrnBelow;
}

void getprev() {
  if (SCRNUM == 0) {
    if (scrnLeft == 0) {
      PREV[0] = PREV[1] = PREV[2] = BLOCK_ID;
      sprev[0] = sprev[1] = sprev[2] = 0;
    }
    return;
  }

  if (scrnLeft == 0) {
    return;
  }

  calcblue(); // Calcul de l’adresse mémoire de l’écran de gauche

  // Récupération des objets et états aux positions 9, 19 et 29
  y = 9;
  PREV[0] = getobjid1();
  sprev[0] = getState();

  y = 19;
  PREV[1] = getobjid1();
  sprev[1] = getState();

  y = 29;
  PREV[2] = getobjid1();
  sprev[2] = getState();
}

void getbelow() {
  if (rowno < 2) {
    // **ONSCREEN** : On regarde sous les lignes du haut et du milieu
    BELOW[0] = PREV[1];   // Bloc précédent
    SBELOW[0] = sprev[1]; // État du bloc précédent

    yindex += 10; // Aller à la ligne suivante
    for (uint8_t i = 1; i < 10; i++) {
      BELOW[i] = getobjid();
      SBELOW[i] = getState();
      yindex++;
    }
    yindex -= 10; // Restauration de yindex
    return;
  }

  // **OFFSCREEN** : On regarde sous la ligne du bas
  if (scrnBelow == 0) {
    // Il n'y a pas d'écran en dessous → On remplit avec des blocs noirs
    for (uint8_t i = 1; i < 10; i++) {
      BELOW[i] = BLOCK;
    }
  } else {
    calcblue(); // Calcule l'adresse mémoire de l'écran en dessous

    // Remplissage de `BELOW` en ignorant le bloc le plus à droite
    for (uint8_t i = 1; i < 9; i++) {
      yindex = i + 10;
      BELOW[i] = getobjid();
      SBELOW[i] = getState();
    }
  }

  // **Vérification de l'écran en bas à gauche**
  if (scrnBelowL == 0) {
    // Il n'y a pas d'écran en bas à gauche
    BELOW[0] = (level == LEVEL_MAX) ? SPACE : BLOCK;
  } else {
    calcblue(); // Calcule l'adresse mémoire de l'écran en bas à gauche
    yindex = 9;
    BELOW[0] = getobjid();
    SBELOW[0] = getState();
  }

  // **Restauration de l'écran actuel**
  calcblue();
}

uint8_t getobjid1() {
  // **Mode éditeur**
  if (inbuilder) {
    return getobjbldr();
  }

  // **Récupération de l'état de l'objet**
  state = BlueSpec[y];

  // **Extraction de l'ID**
  objid = BlueType[y] & IDMASK;

  // **Cas particulier : Plaque de pression normale**
  if (objid == PRESSPLATE) {
    uint8_t link_index = state;
    uint8_t link_state = LINKMAP[link_index] & 0x1F; // Bits 0-4

    if (link_state >= 2) {
      return DPRESSPLATE; // Plaque enfoncée
    }
    return PRESSPLATE; // Plaque relevée
  }

  // **Cas particulier : Plaque de pression supérieure**
  if (objid == UPRESSPLATE) {
    uint8_t link_index = state;
    uint8_t link_state = LINKMAP[link_index] & 0x1F; // Bits 0-4

    if (link_state >= 2) {
      state = 0;
      return FLOOR; // Plaque haute enfoncée = sol normal
    }
    return UPRESSPLATE; // Plaque haute relevée
  }

  return objid;
}

void RedBlockSure() {
    // **Dessiner chaque section du bloc**
    drawc();   // C-section (en bas à gauche)
    drawmc();  // Partie mobile de la section C

    drawb();   // B-section (à gauche)
    drawmb();  // Partie mobile de la section B

    drawd();   // D-section (dessus)
    drawmd();  // Partie mobile de la section D

    drawa();   // A-section (devant)
    drawma();  // Partie mobile de la section A

    drawfrnt(); // Dernier plan (superposition)
}