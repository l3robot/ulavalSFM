Utilisateurs de Colosse, supercalculateur de l'Université Laval
--------------------------------------------------------------------------------------

#### Voilà quelques précisions pour installer ulavalSFM sur colosse :

###### Divers

- Se connecter sur CentOS6 !!! Tout est fonctionnel sur CentOS6 puisqu'il y aura migration
- Créez un fichier .ulavalSFM dans $HOME où vous écrirez :
```Bash
#File used by ulavalSFM

RAP:<Votre RAP colosse ici>;
```

###### Module à charger/changer

- module add apps/cmake/2.8.12.1
- module add apps/git/1.8.5.3
- module add libs/mkl/11.1
- module add libs/opencv/2.4.9

###### Installation

- Installer anaconda version Linux dans votre home https://store.continuum.io/cshop/anaconda/

```Bash
git clone https://github.com/LERobot/ulavalSFM
cd <ulavalSFM>/
chmod +x install.sh
source install.sh -colosse
```

