import pytest
import requests
import json
from conftest import APP_HOST, APP_PORT


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

    request_data = {
        "firstWord": "кирилл",
        "firstCode": "ru",
        "secondWord": "kirill",
        "secondCode": "en",
    }

    response = requests.post(
        f"http://{APP_HOST}:{APP_PORT}/translate",
        headers={"Content-Type": "application/json", "Authorization": access_token},
        data=request_data,
    )

    assert response.status_code == 201, f"{response.text}"
    assert response.json()["status"] == True


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
