from DashBoard.models import MemberInfo, MemberImageInfo
from django.core.management.base import BaseCommand
import requests

class Command(BaseCommand):  # ✅ 클래스명은 Command여야 함
    help = '국회의원 사진 데이터를 통합 API에서 가져와 저장합니다.'

    def handle(self, *args, **kwargs):
        API_KEY = '0a630e0247e541f7895cf18260d41846'
        API_ID = 'ALLNAMEMBER'
        base_url = f'https://open.assembly.go.kr/portal/openapi/{API_ID}.do'

        page = 1
        total_updated = 0

        # 모든 의원을 미리 가져와 dict 형태로 매핑 (속도 최적화)
        member_map = {m.member_code: m for m in MemberInfo.objects.all()}
        to_update = []

        while True:
            params = {
                'KEY': API_KEY,
                'Type': 'json',
                'pIndex': page,
                'pSize': 100,
            }

            response = requests.get(base_url, params=params)
            result = response.json()

            # 오류 응답 처리
            if 'RESULT' in result.get(API_ID, [{}])[0]:
                code = result[API_ID][0]['RESULT']['CODE']
                if code != 'INFO-000':
                    print(f"❌ API 오류: {result[API_ID][0]['RESULT']['MESSAGE']}")
                    break

            data_list = result.get(API_ID, [])
            data = None
            for section in data_list:
                if 'row' in section:
                    data = section['row']
                    break

            if not data:
                break

            for item in data:
                code = item.get('NAAS_CD')
                img_url = item.get('NAAS_PIC')

                if not code or not img_url:
                    print(f"⚠️ 생략됨 → NAAS_CD: {code}, IMAGE: {img_url}")
                    continue

                member = member_map.get(code)
                if not member:
                    continue

                try:
                    image_info, created = MemberImageInfo.objects.get_or_create(member=member)
                    image_info.image_url = img_url
                    to_update.append(image_info)
                    total_updated += 1
                    print(f"✅ 저장 시도 → {member.name} → {img_url}")
                except Exception as e:
                    print(f"❌ {code} 저장 중 오류: {e}")
                    continue

            print(f'{page}페이지 완료 - 누적 {total_updated}명 업데이트')
            page += 1

        # 일괄 업데이트로 성능 향상
        MemberImageInfo.objects.bulk_update(to_update, ['image_url'])

        self.stdout.write(self.style.SUCCESS(f'총 {total_updated}명의 국회의원 사진을 저장했습니다.'))
