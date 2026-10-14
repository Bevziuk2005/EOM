<details>
<summary><strong>Лабораторна робота 4. Процеси, системні виклики та fork() / exec() / wait()</strong></summary>

# Лабораторна робота 4. Процеси, системні виклики та fork() / exec() / wait()

**Виконав:** Бевзюк Нікіта, група КН-24
**Середовище:** Ubuntu 24.04, ядро Linux 6.x, x86-64, GCC 13.3.0, strace 6.8
Усі програми збирались так: `gcc -Wall -Wextra program.c -o program` — попереджень немає.
Конкретні PID та адреси в результатах щоразу інші, тому важливі закономірності, а не числа.

**Питання лабораторної:**
1. Як ОС представляє процес і керує ним?
2. Як програма звертається до ядра (system calls)?
3. Як один процес створює інший і запускає в ньому іншу програму?

```
shell ──fork()──► child ──exec()──► нова програма ──exit()──┐
  │                                                          │
  └───────────────────────wait()◄────────────────────────────┘
```

## Головне за 30 секунд

- **Program ≠ process.** Файл `process_info` один, а процесів з нього я запустив три: PID 295, 296, 297.
- **`fork()` повертає два значення:** батьку — PID дитини (387), дитині — `0`. Тому один рядок `pid = fork()` веде двома шляхами.
- **Змінні після `fork()` окремі.** Батько записав `300`, дитина `200`, а адреса `&value` в обох однакова (`0x56110f84c010`).
- **`exec()` не створює процес:** PID до `exec` 506 і після 506, змінилась лише програма.
- **`wait()` забирає статус дитини** (`42`). Без нього завершена дитина висить у стані `Z` (zombie).
- **`fork()` у C це `clone` у ядрі** (`strace` показав `clone(...) = 493`).
- **Сюрприз з буферизацією:** `printf` перед `write` у `output.c` вивів текст **після** `write`, якщо stdout — файл чи pipe. У терміналі порядок звичайний.

## Файли

| Файл | Завдання |
|------|----------|
| `process_info.c` | 1–3: PID, PPID, `/proc` |
| `hello.c` | 4: system calls через `strace` |
| `output.c` | 5: `printf` проти `write` |
| `fork_basic.c` | 6: перший `fork()` |
| `fork_memory.c` | 7: незалежна пам'ять після `fork()` |
| `wait_basic.c` | 9: `wait()` та exit status |
| `exec_basic.c`, `exec_pid.c`, `new_program.c` | 10: `exec()`, PID до і після |
| `fork_exec.c` | 11: `fork` + `exec` + `wait` |
| `launcher.c` | 12: мінімальний запускач |
| `states.c` | 13: стани процесу |
| `zombie.c`, `zombie_fixed.c` | 14: zombie та його усунення |
| `fork_count.c`, `fork_tree.c` | 15: кількість процесів, дерево |
| `process_runner.c` | фінальна програма |

Executable-файли в Git не додані (`.gitignore`).

---

## 1. Program і process: `process_info.c`

```
gcc -Wall -Wextra process_info.c -o process_info
./process_info
```

```
PID:  295
PPID: 292
```

У другому terminal:

```
$ ps -o pid,ppid,state,cmd -C process_info
  PID  PPID S CMD
  295   292 S ./process_info
```

```
$ file process_info
process_info: ELF 64-bit LSB pie executable, x86-64, version 1 (SYSV), dynamically linked, ...
```

**Де знаходиться executable до запуску?** На диску, це звичайний файл у форматі ELF.
**Що таке PID?** Число, яке ядро видає процесу при створенні, щоб його можна було відрізнити від інших.
**Чому PID не властивість executable?** Бо той самий файл можна запустити багато разів, і кожен раз ядро видасть новий PID. Файл лежить незмінним, а PID існує лише поки живий процес.
**Що таке PPID?** PID батьківського процесу, тобто того, хто створив цей процес.
**Хто parent моєї програми?** `bash`, тобто shell, з якого я її запустив (у другому запуску PPID = 323, і `/proc/323/comm` показує `bash`).

## 2. Одна програма, декілька процесів

Запустив `process_info` тричі одночасно:

| Instance | PID | PPID |
|---|---:|---:|
| Process 1 | 295 | 292 |
| Process 2 | 296 | 292 |
| Process 3 | 297 | 292 |

`ps` показав усі три рядки `./process_info` зі станом `S`.

**Чому один executable дав три PID?** Кожен запуск створює окремий процес зі своїм PID, своїм станом і своєю пам'яттю. PPID однаковий (292), бо всі три запустив один і той самий shell.

**Різниця.** *Program* — це файл із кодом і даними, який лежить на диску. *Process* — запущений екземпляр цієї програми: ядро виділило йому PID, адресний простір і стан, і він виконується (або чекає).

## 3. Дослідження процесу через `/proc`

```
$ ls /proc/325
arch_status attr autogroup auxv cgroup clear_refs cmdline comm coredump_filter cpuset
cwd environ exe fd fdinfo ... limits maps mem mountinfo ... stat statm status ...

$ tr '\0' ' ' < /proc/325/cmdline
./process_info

$ grep -E '^(Name|State|Pid|PPid|Threads|VmSize|VmRSS):' /proc/325/status
Name:   process_info
State:  S (sleeping)
Pid:    325
PPid:   323
Threads:        1
VmSize:     2728 kB
VmRSS:      1872 kB

$ readlink /proc/325/exe
.../lab04-processes/process_info
```

Початок `/proc/325/maps` (шляхи скорочено):

```
55f420793000-55f420794000 r--p 00000000 fe:00 950426   .../process_info
55f420794000-55f420795000 r-xp 00001000 fe:00 950426   .../process_info    ← код
55f420795000-55f420796000 r--p 00002000 fe:00 950426   .../process_info
55f420797000-55f420798000 rw-p 00003000 fe:00 950426   .../process_info    ← дані
55f4515ac000-55f4515cd000 rw-p 00000000 00:00 0        [heap]
7f982e800000-7f982e828000 r--p 00000000 fe:00 344580   /usr/lib/x86_64-linux-gnu/libc.so.6
7f982e828000-7f982e9b1000 r-xp 00028000 fe:00 344580   /usr/lib/x86_64-linux-gnu/libc.so.6
```

| Інформація | Результат |
|---|---|
| Process name | `process_info` |
| PID | 325 |
| PPID | 323 (`bash`) |
| State | `S (sleeping)` |
| Executable | `.../lab04-processes/process_info` |

У `maps` видно ту саму картину, що й у Lab 3: код (`r-xp`), дані (`rw-p`), `[heap]` і підключена `libc.so.6`.

**Чи `/proc/PID` є самим процесом?** Ні. Це віртуальна файлова система ядра: файлів на диску немає, ядро формує їхній вміст у момент читання. `/proc/PID` лише «вікно», через яке видно стан процесу.
**Що з `/proc/PID` після завершення процесу?** Каталог зникає. Після завершення `process_info` команда `ls /proc/325` вивела `No such file or directory`.
**Чому `/proc` корисний для вивчення ОС?** Він робить внутрішній стан процесу видимим: ім'я, стан, PPID, пам'ять, виконуваний файл, без спеціальних інструментів.

---

## 4. System calls: `hello.c`

```
$ ./hello
Hello from process!
```

`strace ./hello` вивів 37 рядків. Фрагмент:

```
execve("./hello", ["./hello"], 0x7ffda3207400 /* 199 vars */) = 0
brk(NULL)                               = 0x556fa60af000
openat(AT_FDCWD, "/lib/x86_64-linux-gnu/libc.so.6", O_RDONLY|O_CLOEXEC) = 3
mmap(NULL, 2174352, PROT_READ, MAP_PRIVATE|MAP_DENYWRITE, 3, 0) = 0x7f0199a00000
...
write(1, "Hello from process!\n", 20)   = 20
exit_group(0)                           = ?
+++ exited with 0 +++
```

Практично всі рядки — це запуск і підготовка (завантаження `libc`, виділення пам'яті), а наш вивід це лише один `write`.

З фільтром `strace -e write ./hello`:

```
write(1, "Hello from process!\n", 20)   = 20
```

(у реальному терміналі текст програми вклинюється всередину рядка strace, бо виводять обидва)

**Інтерпретація `write(1, "Hello from process!\n", 20) = 20`:**
- `1` — file descriptor stdout (стандартний вивід);
- `"Hello from process!\n"` — буфер, дані для запису;
- `20` — кількість байтів (19 символів + `\n`);
- результат `= 20` — скільки байтів ядро реально записало.

Шлях виведення: `printf()` → C library → `write` (system call) → ядро → stdout.

## 5. Library function проти system call: `output.c`

Запуск `strace -e write ./output` залежить від того, куди йде stdout.

**Якщо stdout перенаправлено у файл або pipe** (`./output > o.txt`, `./output | cat`):

```
write(1, "Output using write\n", 19)    = 19
write(1, "Output using printf\n", 20)   = 20
```

**Якщо stdout — термінал** (перевірив через псевдотермінал):

```
write(1, "Output using printf\n", 20)   = 20
write(1, "Output using write\n", 19)    = 19
```

Бачимо два `write`, але **в різному порядку**. Причина — buffering: `printf` не викликає `write` одразу. Якщо stdout — термінал, бібліотека буферизує по рядках і скидає буфер на `\n`, тому `printf` виходить першим. Якщо stdout — файл чи pipe, вона накопичує текст у буфері й скидає його лише при завершенні програми, тож прямий `write` встигає вийти раніше.

**Різниця між `printf` і `write`.** `printf` — функція бібліотеки C: форматує текст, буферизує його і лише потім (можливо, пізніше) звертається до ядра. `write` — прямий system call: дані одразу йдуть у ядро. Тому не можна стверджувати, що кожен `printf` дорівнює одному `write`.

---

## 6. Перший `fork()`: `fork_basic.c`

```
$ ./fork_basic
Before fork: PID=386
PARENT: PID=386, child PID=387, fork returned=387
CHILD:  PID=387, PPID=386, fork returned=0
```

```
Parent PID = 386
Child PID  = 387
Child PPID = 386
```

**Скільки процесів було до `fork()`?** Один.
**Скільки після успішного `fork()`?** Два: parent і child.
**Що отримав parent?** PID дитини (387).
**Що отримав child?** `0`.
**Чому один рядок `pid_t pid = fork();` веде двома шляхами?** `fork()` створює майже точну копію процесу, і обидва процеси продовжують виконання з місця одразу після виклику. Різниця тільки в поверненому значенні: у батька `pid > 0`, у дитини `pid == 0`. Далі `if (pid == 0)` розводить їх по різних гілках.

```
             Process (386)
                │
              fork()
                │
          ┌─────┴─────┐
          ▼           ▼
   Parent (386)   Child (387)
   pid = 387      pid = 0
```

### Порядок виконання

Запустив `./fork_basic` 20 разів і подивився, що друкується першим після `Before fork`:

| Перший надрукував | Разів з 20 |
|---|---:|
| PARENT | 14 |
| CHILD | 6 |

**Чи можна гарантувати порядок?** Ні. Після `fork()` це два незалежні процеси, і який із них отримає процесор першим, вирішує планувальник ОС (scheduler). Це перший приклад concurrency: результат залежить не від коду, а від планування.

## 7. `fork()` і пам'ять: `fork_memory.c`

**Прогноз:** child не побачить `300`, бо в нього своя копія `value`. Він залишиться при своєму `200`.

```
$ ./fork_memory
Before fork: value=100, address=0x56110f84c010
PARENT: PID=472 value=300 address=0x56110f84c010
CHILD:  PID=473 value=200 address=0x56110f84c010
```

Прогноз підтвердився. Ще два запуски (PID прибрано):

```
PARENT: value=300 address=0x563f69e8c010        PARENT: value=300 address=0x55951a193010
CHILD:  value=200 address=0x563f69e8c010        CHILD:  value=200 address=0x55951a193010
```

Адреса `&value` в parent і child **однакова**, а значення різні. Це не суперечність: адреса віртуальна. У кожного процесу свій адресний простір, і та сама віртуальна адреса відображається на різну фізичну пам'ять. Однакове число не означає спільну змінну. (Між запусками адреса змінюється через ASLR із Lab 3, але всередині одного запуску у батька й дитини вона збігається.)

```
Before fork           After fork
Process               Parent         Child
value = 100           value = 300    value = 200
```

## 8. `fork()` через `strace`: `strace -f -e trace=process ./fork_basic`

```
execve("./fork_basic", ["./fork_basic"], ...) = 0
Before fork: PID=492
clone(child_stack=NULL, flags=CLONE_CHILD_CLEARTID|CLONE_CHILD_SETTID|SIGCHLD, ...) = 493
strace: Process 493 attached
PARENT: PID=492, child PID=493, fork returned=493
[pid   492] exit_group(0)               = ?
CHILD:  PID=493, PPID=492, fork returned=0
[pid   492] +++ exited with 0 +++
exit_group(0)                           = ?
+++ exited with 0 +++
```

- створення child: `clone(...) = 493`, а PID дитини — 493;
- `strace: Process 493 attached` — strace почав стежити за новим процесом (опція `-f`);
- завершення: `exit_group(0)` у батька і в дитини.

**Важливе спостереження.** У C ми викликали `fork()`, а в trace немає жодного рядка `fork(`. Є `clone(...)`. `fork()` — це функція бібліотеки (API), а ядро отримує конкретний системний виклик `clone` з набором прапорців. Назви API і системного виклику не обов'язково збігаються.

---

## 9. `wait()` і exit status: `wait_basic.c`

```
$ time ./wait_basic
CHILD: working...
CHILD: finished
PARENT: waiting for child 495
PARENT: child finished
PARENT: child exit status = 42

real    0m2.002s
```

Тут вивід ішов не в термінал, тому текст батька (без `fflush`) скидався з буфера лише при завершенні й «спізнився» за рядками дитини. Це та сама буферизація, що в `output.c`. Порядок можна було б побачити іншим, але логіка не змінюється: `child finished` не з'являється раніше за завершення дитини, і програма працює ~2 с.

Із `strace -f -e trace=process`:

```
clone(...) = 502
[pid   501] wait4(-1,  <unfinished ...>
[pid   502] exit_group(42)              = ?
[pid   502] +++ exited with 42 +++
<... wait4 resumed>[{WIFEXITED(s) && WEXITSTATUS(s) == 42}], 0, NULL) = 502
--- SIGCHLD {si_signo=SIGCHLD, si_code=CLD_EXITED, si_pid=502, ..., si_status=42, ...} ---
```

**Що робить parent під час `wait()`?** Блокується: його стан змінюється на «очікування» (`wait4` не повернувся), він не споживає процесор, доки child не завершиться. Програма працює рівно ~2 секунди, поки спить child.
**Чому «PARENT: child finished» не з'являється раніше?** Бо `wait()` повертає керування лише після завершення дитини. У `strace` видно, що `wait4` «resumed» тільки після `exit_group(42)`.
**Що означає 42?** Exit status: число, яке child повернув із `main` (`return 42`).
**Чи 42 є PID?** Ні. PID дитини — 502, а 42 — код завершення, який дитина обрала сама.
**Навіщо `WIFEXITED` та `WEXITSTATUS`?** `status` у `wait` — закодоване число, в якому зібрано різну інформацію (нормальне завершення чи вбито сигналом, код виходу). `WIFEXITED(status)` перевіряє, що child завершився нормально, а `WEXITSTATUS(status)` витягує з нього код виходу (42).

---

## 10. `exec()`: `exec_basic.c`

```
$ ./exec_basic
Before exec: PID=505
Hello from exec!
```

Рядок `perror("execlp")` не виконався, бо після успішного `exec` код `exec_basic` у цьому процесі більше не існує. Його замінила програма `echo`.

З `strace -e trace=execve`:

```
execve("./exec_basic", ["./exec_basic"], ...) = 0
Before exec: PID=511
execve("/usr/bin/echo", ["echo", "Hello from exec!"], ...) = 0
Hello from exec!
```

(перед цим `execlp` по черзі пробує каталоги зі змінної `PATH`, і в кожному, де `echo` немає, отримує `ENOENT`; тут їх було кілька, скорочено)

**Перевірка головної властивості.** `exec_pid.c` запускає `new_program`, яка теж друкує свій PID:

```
$ ./exec_pid
Before exec PID = 506
New program PID=506
```

```
Before exec PID = 506
After exec PID  = 506
```

| Питання | Відповідь |
|---|---|
| new process? | **ні** |
| new program? | **так** |
| same PID? | **так** |

`exec` не створює процес, а замінює програму, яку виконує поточний процес (код, дані, стек). PID, а також відкриті файли, залишаються.

## 11. `fork()` + `exec()`: `fork_exec.c`

```
$ ./fork_exec
PARENT: PID=512
CHILD before exec: PID=513
Program started by child
PARENT: created child PID=513
PARENT: child completed
```

(рядок «PARENT: created child» міг би стояти й раніше: порядок parent/child не гарантований, а в цьому запуску stdout був pipe, тому текст батька вийшов у кінці)

```
                 Parent (512)
                    │
                  fork()
                    │
            ┌───────┴───────┐
            │               │
          Parent           Child (513)
            │               │
            │             exec()   ← той самий PID 513, але вже echo
            │               │
          wait()          echo "Program started by child"
            │               │
            │             exit(0)
            │               │
            ◄───────────────┘
            │
          continue → "PARENT: child completed"
```

- `fork()` створює дитину: ще одну копію поточного процесу;
- `exec()` у дитині замінює її програму на `echo`, PID лишається 513;
- `wait()` у батька блокує його, доки `echo` не завершиться, і забирає статус.

## 12. Запускач: `launcher.c`

```
$ ./launcher date
Wed Oct 14 ...
Started child PID=515
Child exit status: 0

$ ./launcher whoami
root
Started child PID=517
Child exit status: 0

$ ./launcher uname
Linux
Started child PID=519
Child exit status: 0

$ ./launcher false
Started child PID=521
Child exit status: 1

$ ./launcher nonexistent_prog
execlp: No such file or directory
Started child PID=523
Child exit status: 1

$ ./launcher
Usage: ./launcher <program>
```

Рядок «Started child» стоїть після виводу дочірньої програми лише тому, що stdout у моєму запуску — pipe (буфер батька скидається при завершенні). У терміналі він з'явиться перед виводом дитини.

`launcher` повторює суть роботи shell: прочитати назву програми → `fork()` → у child `exec()` → у parent `wait()` → показати результат. `false` завершується зі статусом 1, а неіснуюча програма призводить до помилки `exec` у дитини, і `_exit(EXIT_FAILURE)` дає статус 1.

---

## 13. Стани процесу: `states.c`

```
$ ./states
PID=527
```

```
$ grep -E '^(Name|State):' /proc/527/status
Name:   states
State:  S (sleeping)

$ ps -o pid,ppid,state,cmd -p 527
  PID  PPID S CMD
  527   526 S ./states

$ awk '{print "utime="$14, "stime="$15}' /proc/527/stat
utime=0 stime=0
```

**Під час `sleep()` процес:** не використовує CPU (utime та stime = 0 тактів), не *running*, а **waiting/sleeping** (стан `S`).

**Чому ОС не повинна витрачати CPU на процес, що чекає таймера?** Він нічого не обчислює: йому просто нема що виконувати. Ядро прибирає його з черги готових до виконання і повертає туди, коли спрацює таймер. Весь цей час процесор вільний для інших процесів. Якби ОС давала йому час, вона б марно витрачала ресурс.

Моделі станів: *Ready* — готовий і чекає процесор, *Running* — зараз виконується на процесорі, *Waiting* — чекає події (таймер, ввід/вивід).

## 14. Zombie: `zombie.c`

Child одразу завершується, а parent 60 секунд спить і `wait()` не викликає.

```
$ ./zombie
PARENT: PID=552, child=553
PARENT: sleeping without wait()
CHILD: PID=553, exiting now
```

Стан дитини:

```
$ ps -o pid,ppid,state,cmd -p 553
  PID  PPID S CMD
  553   552 Z [zombie] <defunct>

$ grep -E '^(Name|State|PPid):' /proc/553/status
Name:   zombie
State:  Z (zombie)
PPid:   552
```

`/proc/553/cmdline` порожній (0 байтів), а `readlink /proc/553/exe` нічого не виводить: програми в процесі вже немає. Батько при цьому живий (`S`).

**Виправлення:** `zombie_fixed.c` викликає `wait(NULL)` (спочатку спить 10 с, щоб було видно zombie):

```
-- до wait() (t≈1.5 с):
  572   571 Z [zombie_fixed] <defunct>

-- після wait() (t≈11.5 с):
PARENT: wait() done, child collected
  PID  PPID S CMD                  ← порожньо, запису про child немає
```

```
Child
 │ exit
 ▼
Terminated ── статус не забрано ──► Zombie
                                       │ parent: wait()
                                       ▼
                                    Removed
```

**Що таке zombie?** Процес, який уже завершився, але в таблиці процесів лишається його запис (PID і exit status), доки parent не викличе `wait()` і не забере цей статус.
**Чи виконує він інструкції?** Ні. Код не виконується і CPU він не споживає, бо програми вже немає. Залишається лише службовий запис, який займає PID.

Якщо parent помирає раніше, child стає «сиротою»: його PPID змінюється на 1 (так було й у моєму експерименті після `kill` батька).

---

## 15. `fork()` і кількість процесів

**Прогноз** для

```c
fork();
fork();
printf("PID=%d\n", getpid());
```

`printf` виконається **4 рази**: перший `fork` перетворює 1 процес на 2, а другий виконують обидва, тому 2 × 2 = 4.

```
$ ./fork_count | wc -l
4
```

Прогноз підтвердився. Для трьох `fork()` вийшло 8 рядків: `2³ = 8`.

| Кількість `fork()` | Процесів |
|---|---:|
| 1 | 2 |
| 2 | 4 |
| 3 | 8 |

### Process tree

`fork_tree.c` те саме, але з `sleep`, щоб батьки були живі, коли діти питають `getppid()` (без цього частина процесів друкує `PPID=1`: їхні батьки вже завершились, і їх «усиновив» процес 1):

```
$ ./fork_tree
PID=679 PPID=672
PID=681 PPID=679
PID=682 PPID=679
PID=683 PPID=681
```

```
bash (672)
 └─ P (679)           ← початковий процес
     ├─ C1 (681)      ← дитина від fork #1; сама робить fork #2
     │   └─ C3 (683)  ← онук
     └─ C2 (682)      ← дитина від fork #2 батька
```

Ще два запуски дали таку саму форму з іншими PID: `685 → 687, 688; 687 → 689` та `690 → 692, 693; 692 → 694`. Структура щоразу «батько → двоє дітей → один онук».

---

## 16. Фінальна програма: `process_runner.c`

Програма: показує PID батька → `fork()` → у child друкує PID і PPID та через `execvp` запускає передану програму (з аргументами, якщо вони є) → у parent `waitpid()` → розбирає, як завершився child (нормально, сигналом чи інакше) → продовжує. Помилки `fork`, `exec` і `waitpid` перевіряються. Якщо `exec` не вдався, child завершується зі статусом `127` (так само роблять shell).

```
$ ./process_runner date
Parent PID: 627
Creating child...

Child PID: 628
Child PPID: 627
Executing: date

Wed Oct 14 ...

Child 628 completed
Exit status: 0

Parent continues
```

Інші випадки:

```
$ ./process_runner echo hi there      → Executing: echo / hi there / Exit status: 0
$ ./process_runner false              → Exit status: 1
$ ./process_runner no_such_program
exec no_such_program: No such file or directory
Child 638 completed
Exit status: 127
$ ./process_runner sh -c 'kill -9 $$'
Child 641 completed
Killed by signal: 9 (Killed)
$ ./process_runner                    → Usage: ./process_runner PROGRAM [args...]
```

### `strace -f -e trace=process ./process_runner date`

Три потрібні фрагменти (рядки про пошук у `PATH` скорочено):

**Створення процесу:**
```
clone(child_stack=NULL, flags=CLONE_CHILD_CLEARTID|CLONE_CHILD_SETTID|SIGCHLD, ...) = 650
```

**Запуск нового program image:**
```
[pid   650] execve("/usr/bin/date", ["date"], ...) = 0
```

**Очікування і завершення child:**
```
wait4(650,  <unfinished ...>
[pid   650] exit_group(0)               = ?
<... wait4 resumed>[{WIFEXITED(s) && WEXITSTATUS(s) == 0}], 0, NULL) = 650
--- SIGCHLD {si_signo=SIGCHLD, si_code=CLD_EXITED, si_pid=650, si_status=0, ...} ---
```

Зіставлення з кодом:

| Код C | Системний виклик ядра |
|---|---|
| `fork()` | `clone(...) = 650` |
| `execvp("date", ...)` | `execve("/usr/bin/date", ...)` |
| `waitpid(pid, &status, 0)` | `wait4(650, ...)` |
| `return` / `_exit` | `exit_group(0)` |
| `WIFEXITED`, `WEXITSTATUS` | розбір статусу `WIFEXITED(s) && WEXITSTATUS(s) == 0` |

Є і додатковий результат: коли child завершується, батькові приходить сигнал `SIGCHLD`. Так ядро повідомляє про завершення дитини.

---

## Відповіді на контрольні питання

1. **Program і process.** Program — файл на диску з кодом і даними. Process — запущений екземпляр програми з PID, пам'яттю та станом.
2. **PID.** Числовий ідентифікатор процесу, який видає ядро.
3. **PPID.** PID батьківського процесу, який створив поточний.
4. **Чому один executable має декілька PID?** Кожен запуск створює окремий процес (у нас три запуски `process_info` дали PID 295, 296, 297).
5. **Роль `/proc/PID`.** Віртуальний каталог ядра, через який видно стан процесу: ім'я, стан, PPID, `exe`, `maps`, `cmdline`.
6. **System call.** Запит програми до ядра ОС на виконання привілейованої дії (запис, створення процесу, запуск програми).
7. **Function call проти system call.** Звичайна функція виконується в просторі програми. System call передає керування ядру. Бібліотечна функція (`printf`) може викликати `write` усередині, але не обов'язково щоразу.
8. **Роль `strace`.** Показує, які системні виклики робить програма, з аргументами та результатами.
9. **`fork()`.** Створює дочірній процес, майже точну копію поточного.
10. **Скільки процесів після одного `fork()`?** Два.
11. **Чому `fork()` повертає 0 у child?** Щоб процеси могли розрізнити себе: дитині 0, а батькові PID дитини (щоб потім на неї чекати). Свій PID дитина дізнається через `getpid()`, а PID батька через `getppid()`.
12. **Що отримує parent?** PID дитини (387 у нашому запуску).
13. **Чи спільна `value`?** Ні. Після `fork()` у кожного своя копія (батько 300, дитина 200).
14. **Чому однакові virtual addresses не означають спільну пам'ять?** Адреси віртуальні й діють у межах свого адресного простору. Однакове число в двох процесах відображається на різну фізичну пам'ять (`0x56110f84c010` у батька й дитини, але значення 300 і 200).
15. **`exec()`.** Замінює програму поточного процесу новою: код, дані, стек.
16. **Чи створює `exec()` новий процес?** Ні.
17. **PID після успішного `exec()`.** Не змінюється (506 до і після).
18. **Навіщо `wait()`.** Щоб parent дочекався завершення child, синхронізувався з ним і забрав exit status. Без цього залишається zombie.
19. **Exit status.** Код, з яким процес завершився (`return 42`). Нуль зазвичай означає успіх.
20. **Zombie.** Завершений процес, чий exit status ще не зібраний батьком (`Z`, `<defunct>`).
21. **Чи виконує zombie інструкції?** Ні, не виконує й не споживає CPU. Залишається лише запис про завершення.
22. **Ready, Running, Waiting.** Ready: процес готовий і чекає процесор. Running: виконується на процесорі зараз. Waiting: чекає події (таймер, ввід/вивід), процесор не потрібен (`State: S`).
23. **Чому порядок parent/child не гарантований?** Після `fork()` це два незалежні процеси, і планувальник сам вирішує, якому дати процесор першим (у 20 запусках парент був першим 14 разів, а дитина 6).
24. **Як `fork → exec → wait` пов'язаний із shell?** Shell так запускає кожну команду: `fork()` створює дитину, у ній `exec()` запускає програму (`date`, `ls`), а shell робить `wait()` і лише потім показує наступне запрошення. Наші `launcher` та `process_runner` реалізують саме це.

---

## Короткий висновок

Експериментально встановлено:

- програма це файл, а процес його запущений екземпляр: один `process_info` дав три різні PID, а `/proc/PID` показує стан живого процесу й зникає після його завершення;
- програма звертається до ядра через system calls: увесь `Hello from process!` це один `write(1, ..., 20)` серед 37 системних викликів; `printf` лише використовує `write` через буфер, тому порядок виводу залежить від того, куди йде stdout;
- `fork()` створює копію процесу з окремим адресним простором: значення `300` і `200` різні, хоч адреса `&value` однакова; на рівні ядра це виклик `clone`;
- порядок parent/child не гарантований: у 20 запусках батько був першим 14 разів, а дитина 6;
- `exec()` замінює програму в тому самому процесі: PID 506 до і після;
- `wait()` блокує батька, повертає статус дитини (`42`), а без нього завершена дитина лишається zombie (`Z`);
- два послідовні `fork()` дають 4 процеси, три дають 8 (`2ⁿ`);
- `fork → exec → wait` це основа роботи shell, і `launcher.c` та `process_runner.c` її відтворюють, що підтвердив `strace`: `clone` → `execve` → `wait4` → `exit_group`.

</details>
