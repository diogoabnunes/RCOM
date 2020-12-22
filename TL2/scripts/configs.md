# Limpar Configurações

* **Começa por ligar todas as portas eth0 (tux2/3/4) ao switch e o S0 do tux3 à consola do switch:**
```
TUX43E0  -> Switch Porta 1 
TUX42E0  -> Switch Porta 2
TUX44E0  -> Switch Porta 3

TUX43S0  -> T3
T4 -> Switch Console
```

* **Inicia o GtkTerm e faz os comandos:**
```
configure terminal
no vlan 2-4094
exit
copy flash:tuxy-clean startup-config
reload
```

* **Assim que isso estiver, troca o cabo que liga T4 á consola do switch para ligar à consola do Router**
```
TUX43S0  -> T3
T4 -> Router Console
```

* **No GtkTerm verifica que estás ligado ao router e faz 2 comandos**
```
copy flash:tuxy-clean startup-config
reload
```

* **Fazer em cada tux**
```
updateimage
service networking restart
```