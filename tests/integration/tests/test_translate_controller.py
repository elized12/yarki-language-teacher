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


@pytest.mark.endpoint("/translate")
def test_auth_filter():

    access_token = get_access_token()

    request_data = json.dumps(
        {
            "firstWord": "кирилл",
            "firstCode": "ru",
            "secondWord": "kirill",
            "secondCode": "en",
        }
    )

    response = requests.post(
        f"http://{APP_HOST}:{APP_PORT}/translate",
        headers={"Content-Type": "application/json", "Authorization": access_token},
        data=request_data.encode("utf-8"),
    )

    assert response.status_code == 500, f"{response.text}"


@pytest.mark.endpoint("/translate", "/translate/{1}")
def test_auth_filter_withoutHeaderAuthorization():
    response = requests.post(
        f"http://{APP_HOST}:{APP_PORT}/translate",
        headers={"Content-Type": "application/json"},
    )

    assert response.status_code == 400, response.text
    assert response.json()["status"] == False

    response = requests.get(
        f"http://{APP_HOST}:{APP_PORT}/translate/1",
        headers={"Content-Type": "application/json"},
    )

    assert response.status_code == 400, response.text
    assert response.json()["status"] == False


@pytest.mark.endpoint("/translate")
def test_add_translate():
    access_token = get_access_token()

    create_language_table()

    request_data = json.dumps(
        {
            "firstWord": "кирилл",
            "firstCode": "ru",
            "secondWord": "kirill",
            "secondCode": "en",
        }
    )

    response = requests.post(
        f"http://{APP_HOST}:{APP_PORT}/translate",
        headers={"Content-Type": "application/json", "Authorization": access_token},
        data=request_data.encode("utf-8"),
    )

    assert response.status_code == 201, f"{response.text}"

    clean_language_table()


@pytest.mark.endpoint("/translate")
def test_add_translate_translateAlreadyExist():
    access_token = get_access_token()
    create_language_table()

    request_data = json.dumps(
        {
            "firstWord": "кирилл",
            "firstCode": "ru",
            "secondWord": "kirill",
            "secondCode": "en",
        }
    )

    response = requests.post(
        f"http://{APP_HOST}:{APP_PORT}/translate",
        headers={"Content-Type": "application/json", "Authorization": access_token},
        data=request_data.encode("utf-8"),
    )

    assert response.status_code == 201, f"{response.text}"
    assert response.json()["status"] == True

    request_data = json.dumps(
        {
            "firstWord": "кирилл",
            "firstCode": "ru",
            "secondWord": "kirill",
            "secondCode": "en",
        }
    )

    response = requests.post(
        f"http://{APP_HOST}:{APP_PORT}/translate",
        headers={"Content-Type": "application/json", "Authorization": access_token},
        data=request_data.encode("utf-8"),
    )

    assert response.status_code == 409, f"{response.text}"
    assert response.json()["status"] == False

    clean_language_table()


@pytest.mark.endpoint("/translate")
def test_add_translate_wordEmpty():
    access_token = get_access_token()
    create_language_table()

    request_data = json.dumps(
        {
            "firstWord": "",
            "firstCode": "ru",
            "secondWord": "kirill",
            "secondCode": "en",
        }
    )

    response = requests.post(
        f"http://{APP_HOST}:{APP_PORT}/translate",
        headers={"Content-Type": "application/json", "Authorization": access_token},
        data=request_data.encode("utf-8"),
    )

    assert response.status_code == 400, f"{response.text}"
    assert response.json()["status"] == False

    clean_language_table()


@pytest.mark.endpoint("/translate")
def test_add_translate_identicalLanguage():
    access_token = get_access_token()
    create_language_table()

    request_data = json.dumps(
        {
            "firstWord": "кирилл",
            "firstCode": "en",
            "secondWord": "kirill",
            "secondCode": "en",
        }
    )

    response = requests.post(
        f"http://{APP_HOST}:{APP_PORT}/translate",
        headers={"Content-Type": "application/json", "Authorization": access_token},
        data=request_data.encode("utf-8"),
    )

    assert response.status_code == 400, f"{response.text}"
    assert response.json()["status"] == False

    clean_language_table()


@pytest.mark.endpoint("/translate")
def test_remove_translate():
    access_token = get_access_token()
    create_language_table()

    request_data = json.dumps(
        {
            "firstWord": "кирилл",
            "firstCode": "ru",
            "secondWord": "kirill",
            "secondCode": "en",
        }
    )

    response = requests.post(
        f"http://{APP_HOST}:{APP_PORT}/translate",
        headers={"Content-Type": "application/json", "Authorization": access_token},
        data=request_data.encode("utf-8"),
    )

    assert response.status_code == 201, f"{response.text}"
    assert response.json()["status"] == True

    data = {
        "firstWordId": response.json()["firstWord"]["id"],
        "secondWordId": response.json()["secondWord"]["id"],
    }

    response = requests.delete(
        f"http://{APP_HOST}:{APP_PORT}/translate/{data['firstWordId']}/{data['secondWordId']}",
        headers={"Content-Type": "application/json", "Authorization": access_token}
    )

    assert response.status_code == 200, response.text
    assert response.json()["status"] == True

    clean_language_table()


@pytest.mark.endpoint("/translate")
def test_remove_translate_notExistTranslate():
    access_token = get_access_token()
    create_language_table()


    response = requests.delete(
        f"http://{APP_HOST}:{APP_PORT}/translate/1/2",
        headers={"Content-Type": "application/json", "Authorization": access_token}
    )

    assert response.status_code == 404, response.text
    assert response.json()["status"] == False

    clean_language_table()


@pytest.mark.endpoint("/translate/{1}")
def test_get_translates():
    access_token = get_access_token()
    create_language_table()

    request_data = json.dumps(
        {
            "firstWord": "кирилл",
            "firstCode": "ru",
            "secondWord": "kirill",
            "secondCode": "en",
        }
    )

    response = requests.post(
        f"http://{APP_HOST}:{APP_PORT}/translate",
        headers={"Content-Type": "application/json", "Authorization": access_token},
        data=request_data.encode("utf-8"),
    )

    assert response.status_code == 201, f"{response.text}"
    assert response.json()["status"] == True

    request_data = json.dumps(
        {
            "firstWord": "кирилл",
            "firstCode": "ru",
            "secondWord": "krutoi",
            "secondCode": "en",
        }
    )

    response = requests.post(
        f"http://{APP_HOST}:{APP_PORT}/translate",
        headers={"Content-Type": "application/json", "Authorization": access_token},
        data=request_data.encode("utf-8"),
    )

    assert response.status_code == 201, f"{response.text}"
    assert response.json()["status"] == True

    response = requests.get(
        f"http://{APP_HOST}:{APP_PORT}/translate/1",
        headers={"Content-Type": "application/json", "Authorization": access_token},
    )

    assert response.status_code == 200, f"{response.text}"
    assert response.json()["status"] == True

    assert response.json()["words"][0]["word"] == "kirill"
    assert response.json()["words"][0]["code"] == "EN"

    assert response.json()["words"][1]["word"] == "krutoi"
    assert response.json()["words"][1]["code"] == "EN"

    clean_language_table()


@pytest.mark.endpoint("/translate/{1}")
def test_get_translates_notExistWord():
    access_token = get_access_token()
    create_language_table()

    response = requests.get(
        f"http://{APP_HOST}:{APP_PORT}/translate/1",
        headers={"Content-Type": "application/json", "Authorization": access_token},
    )

    assert response.status_code == 400, response.text

    clean_language_table()
