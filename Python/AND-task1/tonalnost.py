import re
import pandas as pd
import pymorphy2
import os

df_tone = pd.read_csv("words_all_full_rating.csv", sep=";", encoding="windows-1251")
tone_dict = dict(zip(df_tone["Words"], df_tone["mean"].str.replace(",", ".").astype(float)))
extra_entries = {}
for word, val in tone_dict.items():
    if word.endswith("о"): 
        base = word[:-1] + "ый"
        extra_entries[base] = val
    elif word.endswith("ый") or word.endswith("ий") or word.endswith("ой"):
        base = word[:-2] + "о" 
        extra_entries[base] = val
tone_dict.update(extra_entries)
morph = pymorphy2.MorphAnalyzer()

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
    j = 0
    for w in merged:
        if w.startswith("не_"):
            final.append(f"не_{lemmas[j]}")
        else:
            final.append(lemmas[j])
        j += 1
    return final

def lemmatize_word(word: str) -> str:
    return morph.parse(word)[0].normal_form

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

texts_test = {
    "text1": "Это замечательный день, я счастлив круто шикарно!",
    "text2": "Фильм был ужасный глупый и плохой, не понравился.",
    "text3": "Погода сегодня хорошая, настроение прекрасное.",
    "text4": "Сегодня был замечательный день! Погода просто прекрасная, но работа оказалась сложной и утомительной. Вечером посмотрел отличный фильм, который поднял настроение.",
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
texts_dir = "texts"
texts = {}
for filename in os.listdir(texts_dir):
    if filename.endswith(".txt"):
        filepath = os.path.join(texts_dir, filename)
        with open(filepath, "r", encoding="windows-1251", errors="ignore") as f:
            texts[filename] = f.read()
if texts:
    results_files = []
    for name, text in texts.items():
        lemmas = preprocess_text_with_negation(text)
        stats = text_statistics_numeric(lemmas, tone_dict)
        stats["text_name"] = name
        results_files.append(stats)
    df_files = pd.DataFrame(results_files)[[
        "text_name", "total_words", "tone_words_total", "unique_words", "avg_tone_all", "avg_tone_colored"
    ]]
    print("\nТаблица для файлов")
    print(df_files.to_string(index=False))
    print("\nТональность текстов-файлов")
    for res in results_files:
        tone_label = interpret_tone(res["avg_tone_colored"])
        print(f"{res['text_name']}: {tone_label}, средняя оценка по всем словам {res['avg_tone_all']:.2f}, "
              f"только по окрашенным {res['avg_tone_colored']:.2f}")
else:
    print("\n(Папка 'texts' пуста или не найдена)")
