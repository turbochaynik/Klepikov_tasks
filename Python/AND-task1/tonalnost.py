import re
import pandas as pd
import pymorphy2
import os

morph = pymorphy2.MorphAnalyzer()

if not os.path.exists("words_all_full_rating.csv"):
    print("Файл словаря тональности (words_all_full_rating.csv) не найден! Завершение программы.")
    exit()

df_tone = pd.read_csv("words_all_full_rating.csv", sep=";", encoding="windows-1251")
tone_dict = dict(zip(df_tone["Words"], df_tone["mean"].str.replace(",", ".").astype(float)))

extra_entries = {}
for word, val in tone_dict.items():
    if word.endswith("о"):
        extra_entries[word[:-1] + "ый"] = val
    elif word.endswith(("ый", "ий", "ой")):
        extra_entries[word[:-2] + "о"] = val
tone_dict.update(extra_entries)

def preprocess_text_with_negation(text: str):
    text = text.lower()
    text = re.sub(r'[^а-яё\s]', ' ', text)
    words = text.split()
    merged = []
    skip_next = False
    for i in range(len(words)):
        if skip_next:
            skip_next = False
            continue
        if words[i] == "не" and i + 1 < len(words):
            merged.append(f"не_{words[i + 1]}")
            skip_next = True
        else:
            merged.append(words[i])
    lemmas = [morph.parse(w.replace("не_", ""))[0].normal_form for w in merged]
    final = []
    for j, w in enumerate(merged):
        if w.startswith("не_"):
            final.append(f"не_{lemmas[j]}")
        else:
            final.append(lemmas[j])
    return final


def text_statistics_numeric(lemmas, tone_dict):
    total_words = len(lemmas)
    unique_words = len(set(lemmas))
    tone_score_colored = 0
    tone_count = 0
    tone_score_all = 0
    tone_words = []
    for lemma in lemmas:
        if lemma.startswith("не_"):
            base = lemma[3:]
            score = -tone_dict.get(base, 0)
            tone_score_all += score
            if base in tone_dict:
                tone_score_colored += score
                tone_count += 1
                tone_words.append(lemma)
        else:
            score = tone_dict.get(lemma, 0)
            tone_score_all += score
            if lemma in tone_dict:
                tone_score_colored += score
                tone_count += 1
                tone_words.append(lemma)
    avg_tone_all = tone_score_all / total_words if total_words else 0
    avg_tone_colored = tone_score_colored / tone_count if tone_count else 0
    return {
        "total_words": total_words,
        "tone_words_total": tone_count,
        "unique_words": unique_words,
        "avg_tone_all": avg_tone_all,
        "avg_tone_colored": avg_tone_colored,
        "tone_words": tone_words
    }


def interpret_tone(avg_tone: float) -> str:
    if avg_tone > 0.5:
        return "Очень положительная"
    elif 0.5 >= avg_tone >= 0.1:
        return "Положительная"
    elif avg_tone < -0.5:
        return "Очень отрицательная"
    elif avg_tone < -0.1:
        return "Отрицательная"
    else:
        return "Нейтральная"


print("Выберите режим работы:")
print("1 - Оценить введённый текст или файл")
print("2 - Оценить отзывы из rew.csv")
print("3 - Посмотреть мои тесты")
choice = input("Введите 1, 2 или 3: ").strip()

if choice == "1":
    text_input = input("Введите текст для анализа или имя .txt файла: ").strip()
    from_file = False
    text = None

    if os.path.isfile(text_input):
        from_file = True
        try:
            with open(text_input, "r", encoding="utf-8") as f:
                text = f.read()
        except UnicodeDecodeError:
            with open(text_input, "r", encoding="windows-1251", errors="ignore") as f:
                text = f.read()
    elif os.path.isfile(os.path.join("texts", text_input)):
        from_file = True
        try:
            with open(os.path.join("texts", text_input), "r", encoding="utf-8") as f:
                text = f.read()
        except UnicodeDecodeError:
            with open(os.path.join("texts", text_input), "r", encoding="windows-1251", errors="ignore") as f:
                text = f.read()
    else:
        text = text_input

    lemmas = preprocess_text_with_negation(text)
    stats = text_statistics_numeric(lemmas, tone_dict)
    tone_label = interpret_tone(stats["avg_tone_colored"])

    if not from_file and stats["tone_words"]:
        results_test = []
        name = "test"
        lemmas = preprocess_text_with_negation(text)
        stats = text_statistics_numeric(lemmas, tone_dict)
        stats["text_name"] = name
        results_test.append(stats)
        df_test = pd.DataFrame(results_test)[[
        "text_name", "total_words", "tone_words_total", "unique_words", "avg_tone_all", "avg_tone_colored"
        ]]
        print(df_test)

    print(f"Тональность: {tone_label}")
    print(f"Средняя оценка по окрашенным словам: {stats['avg_tone_colored']:.2f}")
    print(f"Средняя оценка по всем словам: {stats['avg_tone_all']:.2f}")

    if not from_file and stats["tone_words"]:
        print("Окрашенные слова:", ", ".join([w.replace("_", " ") for w in stats["tone_words"]]))

elif choice == "2":
    if not os.path.exists("rew.csv"):
        print("Файл с отзывами (rew.csv) не найден! Завершение программы.")
        exit()
    df_reviews = pd.read_csv("rew.csv", sep=",", encoding="utf-8", header=0, index_col=False)

    df_reviews.columns = df_reviews.columns.str.strip()
    df_reviews["ProductId"] = df_reviews["ProductId"].astype(str).str.strip().str.upper()
    print("Доступные ProductId:", df_reviews["ProductId"].unique())
    product_id = input("Введите ProductId товара: ").strip().upper()
    df_product = df_reviews[df_reviews["ProductId"] == product_id]

    if df_product.empty:
        print("Отзывы для данного товара не найдены.")
        print("Доступные ProductId:", df_reviews["ProductId"].unique())
    else:
        tone_results = []
        print(f"\nОтзывы для товара {product_id}:\n")
        for idx, row in df_product.iterrows():
            text = str(row["Text"])
            lemmas = preprocess_text_with_negation(text)
            stats = text_statistics_numeric(lemmas, tone_dict)
            tone_label = interpret_tone(stats["avg_tone_colored"])
            tone_results.append(stats["avg_tone_colored"])
            print(f"Отзыв от {row['ProfileName']} (Оценка: {row['Score']}):")
            print(f"Текст: {text}")
            print(f"Тональность: {tone_label}, средняя оценка по окрашенным словам: {stats['avg_tone_colored']:.2f}")
            print(f"Окрашенные слова: {', '.join([w.replace('_',' ') for w in stats['tone_words']]) if stats['tone_words'] else '—'}")
            print("-"*80)

        avg_text_tone = sum(tone_results)/len(tone_results)
        print(f"\nИтоговая оценка тональности текстов: {interpret_tone(avg_text_tone)}, среднее значение: {avg_text_tone:.2f}")
        avg_score = df_product["Score"].astype(float).mean()
        print(f"Средняя числовая оценка всех отзывов: {avg_score:.2f}")
elif choice == "3":
    texts_test = {
    "text1": "Это замечательный день, я счастлив круто шикарно!",
    "text2": "Фильм был ужасный глупый и плохой, не понравился.",
    "text3": "Погода сегодня неплохая, только воздух слишком душный.",
    "text4": "Я думаю это не смешно и скучно, хотя кому то нравится",
    "text5": "Я хожу по лесу.",
    "text6": "Впервые постирал футболку и принт сразу стерся. Спасибо за такое 'шикарное' качество, я очень доволен.",
    "text7": "'шикарно', 'прекрасно', 'красиво' - этими словами описывают город Самара. Я с этими словами полностью согласен.",
    "text8": "Было бы неплохо, если бы исправили ошибку."
    }
    results_test = []
    for name, text in texts_test.items():
        lemmas = preprocess_text_with_negation(text)
        stats = text_statistics_numeric(lemmas, tone_dict)
        stats["text_name"] = name
        results_test.append(stats)

    df_test = pd.DataFrame(results_test)[[
        "text_name", "total_words", "tone_words_total", "unique_words", "avg_tone_all", "avg_tone_colored"
    ]]
    print(df_test)
    print("\nТональность тестовых текстов")
    for res in results_test:
        tone_label = interpret_tone(res["avg_tone_colored"])
        text_original = texts_test[res["text_name"]]
        print(f"Текст: {text_original}")
        print(f"{res['text_name']}: {tone_label}, средняя оценка по всем словам {res['avg_tone_all']:.2f}, "
            f"только по окрашенным {res['avg_tone_colored']:.2f}")
        print(f"Окрашенные слова: {', '.join([w.replace('_', ' ') for w in res['tone_words']]) if res['tone_words'] else '—'}")
        print()
else:
    print("Неверный выбор. Завершение программы.")
