# Сборка CPSGuard через GitHub Actions

1. Создай новый GitHub repository.
2. Загрузи туда содержимое этого проекта.
3. Открой вкладку **Actions**.
4. Выбери **Build CPSGuard**.
5. Нажми **Run workflow**.
6. После окончания открой завершившийся workflow.
7. Внизу страницы скачай artifact **endstone_cpsguard**.
8. Внутри будет `endstone_cpsguard.so`.

Сборка выполняется на Ubuntu GitHub runner и автоматически устанавливает:
- Clang 21
- libc++ 21
- libc++abi 21
- CMake
- Ninja

Для сборки используется Endstone 0.11.12 из CMake проекта CPSGuard.
