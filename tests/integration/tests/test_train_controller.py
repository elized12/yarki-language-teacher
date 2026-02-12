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


def create_train_mode():
    with conn.cursor() as curr:
        curr.execute(
            "INSERT INTO train_card_mode (name) VALUES ('text_input'), ('cards');"
        )
        conn.commit()


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


@pytest.mark.endpoint("/train/session")
def test_start_train_session():
    access_token = get_access_token()
    create_language_table()

    body = json.dumps({"source_language": "ru", "target_language": "en"})

    response = requests.post(
        f"http://{APP_HOST}:{APP_PORT}/train/session",
        headers={"Content-Type": "application/json", "Authorization": access_token},
        data=body.encode("utf-8"),
    )

    assert response.status_code == 201, f"{response.text}"
    assert response.json()["status"] == True
    assert "sessionId" in response.json()

    clean_language_table()


@pytest.mark.endpoint("/train/session/{sessionId}/task")
def test_get_task_andSendAnswerFlow():
    access_token = get_access_token()
    create_language_table()
    create_train_mode()

    request_data = json.dumps(
        {
            "firstWord": "слово",
            "firstCode": "ru",
            "secondWord": "word",
            "secondCode": "en",
        }
    )

    r = requests.post(
        f"http://{APP_HOST}:{APP_PORT}/translate",
        headers={"Content-Type": "application/json", "Authorization": access_token},
        data=request_data.encode("utf-8"),
    )

    assert r.status_code == 201, r.text

    body = json.dumps({"source_language": "ru", "target_language": "en"})

    r = requests.post(
        f"http://{APP_HOST}:{APP_PORT}/train/session",
        headers={"Content-Type": "application/json", "Authorization": access_token},
        data=body.encode("utf-8"),
    )
    assert r.status_code == 201, r.text
    session_id = r.json().get("sessionId")
    assert session_id

    r = requests.get(
        f"http://{APP_HOST}:{APP_PORT}/train/session/{session_id}/task",
        headers={"Content-Type": "application/json", "Authorization": access_token},
    )
    assert r.status_code == 200, r.text
    assert r.json()["status"] == True
    card = r.json()["card"]
    card_id = card["id"]

    answer_body = json.dumps({"answer": "word"})
    r = requests.post(
        f"http://{APP_HOST}:{APP_PORT}/train/session/{card_id}/answer",
        headers={"Content-Type": "application/json", "Authorization": access_token},
        data=answer_body.encode("utf-8"),
    )
    assert r.status_code == 200, r.text
    assert r.json()["status"] == True
    assert r.json()["is_correct"] == True

    clean_language_table()


@pytest.mark.endpoint("/train/session/{sessionId}/task")
def test_get_task_withoutAuth():
    create_language_table()
    create_train_mode()

    r = requests.get(
        f"http://{APP_HOST}:{APP_PORT}/train/session/1/task",
        headers={"Content-Type": "application/json"},
    )

    assert r.status_code == 400, r.text
    assert r.json()["status"] == False

    clean_language_table()


@pytest.mark.endpoint("/train/session/finish")
def test_finish_session_flow():
    access_token = get_access_token()
    create_language_table()

    body = json.dumps({"source_language": "ru", "target_language": "en"})

    r = requests.post(
        f"http://{APP_HOST}:{APP_PORT}/train/session",
        headers={"Content-Type": "application/json", "Authorization": access_token},
        data=body.encode("utf-8"),
    )
    assert r.status_code == 201, r.text
    session_id = r.json().get("sessionId")
    assert session_id

    finish_body = json.dumps({"sessionId": session_id})
    r = requests.post(
        f"http://{APP_HOST}:{APP_PORT}/train/session/finish",
        headers={"Content-Type": "application/json", "Authorization": access_token},
        data=finish_body.encode("utf-8"),
    )
    assert r.status_code == 200, r.text
    assert r.json()["status"] == True

    r = requests.post(
        f"http://{APP_HOST}:{APP_PORT}/train/session/finish",
        headers={"Content-Type": "application/json", "Authorization": access_token},
        data=finish_body.encode("utf-8"),
    )
    assert r.status_code == 400
    assert r.json()["status"] == False

    clean_language_table()


@pytest.mark.endpoint("/train/session/finish")
def test_finish_session_badRequest():
    access_token = get_access_token()
    create_language_table()

    r = requests.post(
        f"http://{APP_HOST}:{APP_PORT}/train/session/finish",
        headers={"Content-Type": "application/json", "Authorization": access_token},
        data=json.dumps({}).encode("utf-8"),
    )
    assert r.status_code == 400, r.text
    assert r.json()["status"] == False

    clean_language_table()


@pytest.mark.endpoint("/train/session/finish")
def test_finish_session_nonExistent():
    access_token = get_access_token()
    create_language_table()

    finish_body = json.dumps({"sessionId": "999999"})
    r = requests.post(
        f"http://{APP_HOST}:{APP_PORT}/train/session/finish",
        headers={"Content-Type": "application/json", "Authorization": access_token},
        data=finish_body.encode("utf-8"),
    )
    assert r.status_code == 404, r.text
    assert r.json()["status"] == False

    clean_language_table()


@pytest.mark.endpoint("/train/session/{1:cardId}/answer")
def test_answer_incorrectAndCardNotFound():
    access_token = get_access_token()
    create_language_table()
    create_train_mode()

    request_data = json.dumps(
        {
            "firstWord": "слово",
            "firstCode": "ru",
            "secondWord": "word",
            "secondCode": "en",
        }
    )

    r = requests.post(
        f"http://{APP_HOST}:{APP_PORT}/translate",
        headers={"Content-Type": "application/json", "Authorization": access_token},
        data=request_data.encode("utf-8"),
    )
    assert r.status_code == 201, r.text

    body = json.dumps({"source_language": "ru", "target_language": "en"})

    r = requests.post(
        f"http://{APP_HOST}:{APP_PORT}/train/session",
        headers={"Content-Type": "application/json", "Authorization": access_token},
        data=body.encode("utf-8"),
    )
    assert r.status_code == 201, r.text
    session_id = r.json().get("sessionId")
    assert session_id

    r = requests.get(
        f"http://{APP_HOST}:{APP_PORT}/train/session/{session_id}/task",
        headers={"Content-Type": "application/json", "Authorization": access_token},
    )
    assert r.status_code == 200, r.text
    card = r.json()["card"]
    card_id = card["id"]

    answer_body = json.dumps({"answer": "wrong"})
    r = requests.post(
        f"http://{APP_HOST}:{APP_PORT}/train/session/{card_id}/answer",
        headers={"Content-Type": "application/json", "Authorization": access_token},
        data=answer_body.encode("utf-8"),
    )
    assert r.status_code == 200, r.text
    assert r.json()["status"] == True
    assert r.json()["is_correct"] == False

    answer_body = json.dumps({"answer": "x"})
    r = requests.post(
        f"http://{APP_HOST}:{APP_PORT}/train/session/999999/answer",
        headers={"Content-Type": "application/json", "Authorization": access_token},
        data=answer_body.encode("utf-8"),
    )
    assert r.status_code == 404
    assert r.json()["status"] == False

    clean_language_table()


@pytest.mark.endpoint("/train/session/{1:sessionId}/task")
def test_get_task_noTranslates():
    access_token = get_access_token()
    create_language_table()
    create_train_mode()

    body = json.dumps({"source_language": "ru", "target_language": "en"})

    r = requests.post(
        f"http://{APP_HOST}:{APP_PORT}/train/session",
        headers={"Content-Type": "application/json", "Authorization": access_token},
        data=body.encode("utf-8"),
    )
    assert r.status_code == 201, r.text
    session_id = r.json().get("sessionId")
    assert session_id

    r = requests.get(
        f"http://{APP_HOST}:{APP_PORT}/train/session/{session_id}/task",
        headers={"Content-Type": "application/json", "Authorization": access_token},
    )
    assert r.status_code == 404, r.text
    assert r.json()["status"] == False

    clean_language_table()
