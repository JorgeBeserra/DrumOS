# Changelog

Todas as mudanças relevantes do DrumOS serão registradas aqui.

O projeto seguirá versionamento semântico quando as releases forem ativadas.

## Unreleased

### Adicionado

- GitHub Actions para build com PlatformIO.
- Publicação de firmware como artifact do CI.
- Documentação inicial em `docs/`.
- `peakMax` por pad para calibrar velocity.
- Comando `max` na Shell.
- Crosstalk simples no Trigger Engine.

### Alterado

- Velocity agora usa `threshold` e `peakMax` para escalar a dinâmica.
- Configurações iniciais dos pads ajustadas para calibração prática.

### Próximo

- Persistência das configurações.
- Retrigger Cancel.
- Adaptive Scan.
- Crosstalk Matrix.
