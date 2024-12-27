
#--------------------------------------------------------------#
    current version of the modules:

matrix : v0.2.0-beta (tag : matrix-v0.2.0-beta)
segment : v0.1.0 (tag : segment-v0.1.0)
worker : v0.0.2 (tag : worker : v0.0.2)

#--------------------------------------------------------------#
    informations generales:

pour ajouter un TODO de priorite entre 1 et 9 il suffit de mettre
  un commentaire //TODO[1-9] a n'import quel endroit des sources

la liste des TODOS qui sont presents dans les fichiers
  peut etre obtenu a l'aide de la commande 'grep -r TODO .'

les commentaires de la forme /* texte */ sont pour faciliter
  la comprehention du code, ils sont places au debut d'un bloc
  de code ou d'une suite d'instructions a utiliser sans moderation

on fera un programme pour nettoyer les sources des commentaires /**/
  et //TODO apres avoir termine le projet

pour faire des commits de changements dans un module: (flow fait maison)
  $git status
  $git checkout -b nom_du_module # vers une nouvelle branche nom_du_module
  $git add -- src/nom_du_module
  $git commit
    un message de commit
  $git tag nom_du_module-vX1.X2.X3
    (ancien X3) + 1 si correction de bug ou optimisation sans changer le
      fonctionnement du module vu de l'externe (le header n'est pas change).
    (ancien X2) + 1, X3 remis a 0 si introduction de nouvelles fonctions
      ou changement des entetes de fonctions deja existantes.
  $git switch dev
  $git merge --no-ff nom_du_module-vX1.X2.X3
    ou faire une fois "$git config merge.ff false" pour enregistrer la
      configuration pour les prochains git merge
    le tag est celui cree avant le git switch dev
  $git branch -d nom_du_module
    supprimer la branch cree pour effectuer le changement dans le module
  $(mettre a jour les versions dans README.txt)
  $git add README.txt && git commit -m 'updated versions in README.txt.'
  $git push --tags 'origin' dev
    inclure les tags dans le push vers 'origin'
