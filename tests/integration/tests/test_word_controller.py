import pytest
import requests
import json
from conftest import APP_HOST, APP_PORT, conn


def create_language_table():
    with conn.cursor() as curr:
        curr.execute(
            "INSERT INTO language (code, name) VALUES ('ru', 'Russian'), ('en', 'English')"
        )
        conn.commit()


def clean_language_table():
    with conn.cursor() as curr:
        curr.execute("TRUNCATE language RESTART IDENTITY CASCADE")


def get_access_token() -> str:
    email = "test@mail.ru"
    nickname = "test"
    password = "Kirill200415!"

    request_data = json.dumps(
        {"email": email, "nickname": nickname, "password": password}
    )

    response = requests.post(
        f"http://{APP_HOST}:{APP_PORT}/sign-up",
        data=request_data,
        headers={"Content-Type": "application/json"},
    )

    request_data = json.dumps({"email": email, "password": password})

    response = requests.post(
        f"http://{APP_HOST}:{APP_PORT}/sign-in",
        data=request_data,
        headers={"Content-Type": "application/json"},
    )

    return response.json()["access_token"]


def get_user_id() -> int:
    with conn.cursor() as curr:
        curr.execute('SELECT id FROM "user" WHERE email = %s', ("test@mail.ru",))
        result = curr.fetchone()
        return int(result[0]) if result else None  # type: ignore


def create_translate(
    access_token: str,
    first_word: str,
    first_code: str,
    second_word: str,
    second_code: str,
):
    request_data = json.dumps(
        {
            "firstWord": first_word,
            "firstCode": first_code,
            "secondWord": second_word,
            "secondCode": second_code,
        }
    )

    return requests.post(
        f"http://{APP_HOST}:{APP_PORT}/translate",
        headers={"Content-Type": "application/json", "Authorization": access_token},
        data=request_data.encode("utf-8"),
    )


@pytest.mark.endpoint("/words/{userId}/{languageCode}")
def test_get_words():
    access_token = get_access_token()
    create_language_table()

    response = create_translate(access_token, "кирилл", "ru", "kirill", "en")
    assert response.status_code == 201, f"{response.text}"

    response = create_translate(access_token, "привет", "ru", "hello", "en")
    assert response.status_code == 201, f"{response.text}"

    user_id = get_user_id()

    response = requests.get(
        f"http://{APP_HOST}:{APP_PORT}/words/{user_id}/ru?offset=0&limit=10",
        headers={"Content-Type": "application/json", "Authorization": access_token},
    )

    assert response.status_code == 200, f"{response.text}"
    assert response.json()["status"] == True
    assert response.json()["count"] == 2
    assert len(response.json()["words"]) == 2

    clean_language_table()


@pytest.mark.endpoint("/words/{userId}/{languageCode}")
def test_get_words_withoutAuth():
    create_language_table()
    user_id = 1

    response = requests.get(
        f"http://{APP_HOST}:{APP_PORT}/words/{user_id}/ru?offset=0&limit=10",
        headers={"Content-Type": "application/json"},
    )

    assert response.status_code == 400, f"{response.text}"
    assert response.json()["status"] == False

    clean_language_table()


@pytest.mark.endpoint("/words/{userId}/{languageCode}")
def test_get_words_withInvalidlLimit():
    access_token = get_access_token()
    create_language_table()

    response = create_translate(access_token, "кирилл", "ru", "kirill", "en")
    assert response.status_code == 201, f"{response.text}"

    user_id = get_user_id()

    response = requests.get(
        f"http://{APP_HOST}:{APP_PORT}/words/{user_id}/ru?offset=0&limit=5001",
        headers={"Content-Type": "application/json", "Authorization": access_token},
    )

    assert response.status_code == 400, f"{response.text}"
    assert response.json()["status"] == False

    clean_language_table()


@pytest.mark.endpoint("/words/{userId}/{languageCode}")
def test_get_words_withOffsetAndLimit():
    access_token = get_access_token()
    create_language_table()

    symbol = ["a", "b", "c", "d", "g"]

    for i in range(5):
        response = create_translate(
            access_token, f"слово{symbol[i]}", "ru", f"word{symbol[i]}", "en"
        )
        assert response.status_code == 201, f"{response.text}"

    user_id = get_user_id()

    response = requests.get(
        f"http://{APP_HOST}:{APP_PORT}/words/{user_id}/ru?offset=0&limit=2",
        headers={"Content-Type": "application/json", "Authorization": access_token},
    )

    assert response.status_code == 200, f"{response.text}"
    assert response.json()["status"] == True
    assert response.json()["count"] == 2
    assert len(response.json()["words"]) == 2

    response = requests.get(
        f"http://{APP_HOST}:{APP_PORT}/words/{user_id}/ru?offset=2&limit=2",
        headers={"Content-Type": "application/json", "Authorization": access_token},
    )

    assert response.status_code == 200, f"{response.text}"
    assert response.json()["status"] == True
    assert response.json()["count"] == 2
    assert len(response.json()["words"]) == 2

    clean_language_table()


@pytest.mark.endpoint("/words/{userId}/{languageCode}")
def test_get_words_emptyResult():
    access_token = get_access_token()
    create_language_table()

    user_id = get_user_id()

    response = requests.get(
        f"http://{APP_HOST}:{APP_PORT}/words/{user_id}/ru?offset=0&limit=10",
        headers={"Content-Type": "application/json", "Authorization": access_token},
    )

    assert response.status_code == 200, f"{response.text}"
    assert response.json()["status"] == True
    assert response.json()["count"] == 0

    clean_language_table()


@pytest.mark.endpoint("/words/{userId}/{languageCode}")
def test_get_words_languageNotExist():
    access_token = get_access_token()
    create_language_table()

    user_id = get_user_id()

    response = requests.get(
        f"http://{APP_HOST}:{APP_PORT}/words/{user_id}/fr?offset=0&limit=10",
        headers={"Content-Type": "application/json", "Authorization": access_token},
    )

    assert response.status_code == 400, f"{response.text}"
    assert response.json()["status"] == False

    clean_language_table()
