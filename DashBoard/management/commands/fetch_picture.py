from DashBoard.models import MemberInfo, MemberImageInfo
from django.core.management.base import BaseCommand
import requests

class Picture(BaseCommand):
    help = '국회의원 사진 데이터를 통합 API에서 가져와 저장합니다.'

    def handle(self, *args, **kwargs):
        API_KEY = '0a630e0247e541f7895cf18260d41846'
        API_ID = 'ALLNAMEMBER'  # 국회의원 통합정보 API (사진 포함)
        base_url = f'https://open.assembly.go.kr/portal/openapi/{API_ID}.do'

        page = 1
        total_updated = 0

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

                print(f"✅ 저장 시도 → {code} → {img_url}")
                if not code or not img_url:
                    continue

                try:
                    member = MemberInfo.objects.get(member_code=code)
                    MemberImageInfo.objects.update_or_create(
                        member=member,
                        defaults={'image_url': img_url}
                    )
                    total_updated += 1
                except MemberInfo.DoesNotExist:
                    print(f"⚠️ 코드 {code} 해당하는 국회의원이 없습니다.")
                    continue

            print(f'{page}페이지 완료 - 누적 {total_updated}명 업데이트')
            page += 1
            for m in MemberImageInfo.objects.all():
             print(m.member.name, m.image_url)
        self.stdout.write(self.style.SUCCESS(f'총 {total_updated}명의 국회의원 사진을 저장했습니다.'))
