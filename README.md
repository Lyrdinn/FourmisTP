<h1 align="center"> Google AI Challenge (2011) </h1>

<p align="center">
<img alt="Static Badge" src="https://img.shields.io/badge/Made%20in%20C%2B%2B-2d6eb3?style=flat-square&logo=c%2B%2B">
<img alt="Static Badge" src="https://img.shields.io/badge/CNAM_ENJMIN-ea0000?style=flat-square">
</p>

[Google AI Challenge prompt](http://ants.aichallenge.org/).

Contest made in a team of 3 :  [Marina SECORD](https://github.com/Lyrdinn/), [Nino SANDLARZ](https://github.com/Sand-Fox) and [Alvina Damasio--Razafiarizaka](http://ants.aichallenge.org/).

<p>
The goal of this project was to develop an AI that would control a colony of ants. The contest consisted of placing 6 groups of ants inside a map and then
see which one would win. To gain points you have to defend your colony as well as attack the others, ants need to search for food to expand their numbers.

We finished with the 3rd place out of 6.
  
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Implementation :
 - Marina Secord : Basic code for all actions at the start of project and ants' defense formation using BSP search
 - Nino Sandlarz : Ants' pathfinding using A star
 - Alvina Damasio--Razafiarizaka : Ant's pathfinding using A star and basic colony attack
  
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Strategy :
  Our strategy was to develop our ants as fast as possible by having them searching for food until we obtained a certain number of ants. Then we leave a big pack of ants defend our colony's 
  outer walls while sending other ants to attack the others.
